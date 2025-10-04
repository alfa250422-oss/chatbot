cmake_minimum_required(VERSION 3.20)
project(chatbot C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(USE_OPENSSL "Use OpenSSL for TLS (otherwise mbedTLS)" ON)

file(GLOB SRC
        src/main.cpp
        src/audio/*.cpp
        src/codec/*.cpp
        src/net/*.cpp
        src/realtime/*.cpp
        src/safety/*.cpp
        src/utils/*.cpp)

add_executable(chatbot ${SRC})
target_include_directories(chatbot PRIVATE include)


if(WIN32)
  # 用 vcpkg 的 libwebsockets Config 包
  find_package(libwebsockets CONFIG REQUIRED)

  set(LWS_TARGET "")
  foreach(cand
          websockets_shared
          websockets_static
          websockets
          libwebsockets::libwebsockets
          libwebsockets::websockets)
    if(TARGET ${cand})
      set(LWS_TARGET ${cand})
      break()
    endif()
  endforeach()

  if(NOT LWS_TARGET)
    message(FATAL_ERROR "Found libwebsockets but no known CMake target; \tried websockets_shared, websockets_static, websockets, \
              libwebsockets::libwebsockets, libwebsockets::websockets")
  endif()

  find_package(PortAudio CONFIG REQUIRED)
  find_package(opus CONFIG REQUIRED)
  find_package(cjson CONFIG REQUIRED)
  find_package(OpenSSL REQUIRED)

  target_link_libraries(chatbot PRIVATE
          OpenSSL::SSL OpenSSL::Crypto
          ${LWS_TARGET}
          #PortAudio::PortAudio
          Opus::opus
          #cjson::cjson
  )
else()
  # Linux / macOS：保留 pkg-config 流程
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)
  pkg_check_modules(LWS REQUIRED libwebsockets)
  pkg_check_modules(OPUS REQUIRED opus)
  pkg_check_modules(CJSON REQUIRED cjson)
  target_include_directories(chatbot PRIVATE
          ${PORTAUDIO_INCLUDE_DIRS}
          ${LWS_INCLUDE_DIRS}
          ${OPUS_INCLUDE_DIRS}
          ${CJSON_INCLUDE_DIRS}
  )
  target_link_libraries(chatbot
          ${PORTAUDIO_LIBRARIES}
          ${LWS_LIBRARIES}
          ${OPUS_LIBRARIES}
          ${CJSON_LIBRARIES}
  )
  find_package(OpenSSL QUIET)
  if(USE_OPENSSL AND OpenSSL_FOUND)
    target_link_libraries(chatbot PRIVATE OpenSSL::SSL OpenSSL::Crypto)
    target_compile_definitions(chatbot PRIVATE USE_OPENSSL)
  else()
    target_compile_definitions(chatbot PRIVATE USE_MBEDTLS)
  endif()
  if(APPLE)
    target_link_libraries(chatbot "-framework CoreAudio" "-framework AudioToolbox" "-framework AudioUnit" "-framework CoreServices" "-framework Carbon")
  endif()
endif()
