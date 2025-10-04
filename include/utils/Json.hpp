#pragma once
#include <string>
struct cJSON;

namespace json {
  using JSON = cJSON;
  JSON* obj();
  JSON* arr();
  void add(JSON* o, const char* k, const char* v);
  void add(JSON* o, const char* k, int v);
  void add(JSON* o, const char* k, double v);
  void add(JSON* o, const char* k, bool v);
  void add(JSON* o, const char* k, JSON* v);
  std::string str(JSON* o);
  void free(JSON* o);
}