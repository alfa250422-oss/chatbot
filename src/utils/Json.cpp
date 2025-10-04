#include "utils/Json.hpp"
#include <cjson/cJSON.h>
#include <cstdlib>

namespace json {
  JSON* obj(){ return cJSON_CreateObject(); }
  JSON* arr(){ return cJSON_CreateArray(); }
  void add(JSON* o, const char* k, const char* v){ cJSON_AddItemToObject(o,k,cJSON_CreateString(v)); }
  void add(JSON* o, const char* k, int v){ cJSON_AddItemToObject(o,k,cJSON_CreateNumber(v)); }
  void add(JSON* o, const char* k, double v){ cJSON_AddItemToObject(o,k,cJSON_CreateNumber(v)); }
  void add(JSON* o, const char* k, bool v){ cJSON_AddItemToObject(o,k,cJSON_CreateBool(v)); }
  void add(JSON* o, const char* k, JSON* v){ cJSON_AddItemToObject(o,k,v); }
  std::string str(JSON* o){ char* s=cJSON_PrintUnformatted(o); std::string r=s?s:"{}"; if(s) free(s); return r; }
  void free(JSON* o){ cJSON_Delete(o); }
}