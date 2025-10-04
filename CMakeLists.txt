cmake_minimum_required(VERSION 3.16)
project(chatbot C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(USE_OPENSSL "Use OpenSSL for TLS (otherwise mbedTLS)" ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)
pkg_check_modules(LWS REQUIRED libwebsockets)
pkg_check_modules(OPUS REQUIRED opus)
pkg_check_modules(OPENSSL QUIET openssl)
pkg_check_modules(CJSON QUIET cjson)

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

# JSON
if(CJSON_FOUND)
  target_include_directories(chatbot PRIVATE ${CJSON_INCLUDE_DIRS})
  target_link_libraries(chatbot ${CJSON_LIBRARIES})
else()
  message(FATAL_ERROR "cJSON not found. Install libcjson-dev or vendor cJSON sources into thirdparty.")
endif()

# 系统库
target_include_directories(chatbot PRIVATE 
  ${PORTAUDIO_INCLUDE_DIRS}
  ${LWS_INCLUDE_DIRS}
  ${OPUS_INCLUDE_DIRS}
)

target_link_libraries(chatbot 
  ${PORTAUDIO_LIBRARIES}
  ${LWS_LIBRARIES}
  ${OPUS_LIBRARIES}
)

if(USE_OPENSSL AND OPENSSL_FOUND)
  target_include_directories(chatbot PRIVATE ${OPENSSL_INCLUDE_DIRS})
  target_link_libraries(chatbot ${OPENSSL_LIBRARIES})
  target_compile_definitions(chatbot PRIVATE USE_OPENSSL)
else()
  target_compile_definitions(chatbot PRIVATE USE_MBEDTLS)
  # 如使用 mbedTLS: target_link_libraries(chatbot mbedtls mbedcrypto mbedx509)
endif()

if(APPLE)
  target_link_libraries(chatbot "-framework CoreAudio" "-framework AudioToolbox" "-framework AudioUnit" "-framework CoreServices" "-framework Carbon")
endif()