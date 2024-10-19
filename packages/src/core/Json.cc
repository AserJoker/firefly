#include "core/Json.hpp"
#include "core/Value.hpp"
#include <cjson/cJSON.h>
using namespace firefly;
using namespace firefly::core;
static Value cJSONtoValue(cJSON *node) {
  if (cJSON_IsBool(node)) {
    return core::Boolean_t(node->valueint == 1);
  }
  if (cJSON_IsNull(node)) {
    return nullptr;
  }
  if (cJSON_IsNumber(node)) {
    return (core::Float_t)node->valuedouble;
  }
  if (cJSON_IsString(node)) {
    return core::String_t(node->valuestring);
  }
  if (cJSON_IsArray(node)) {
    Array_t result;
    cJSON *child = node->child;
    while (child) {
      result.pushBack(cJSONtoValue(child));
      child = child->next;
    }
    return result;
  }
  if (cJSON_IsObject(node)) {
    Map_t result;
    cJSON *child = node->child;
    while (child) {
      result[child->string] = cJSONtoValue(child);
      child = child->next;
    }
    return result;
  }
  return nullptr;
}
static cJSON *ValuetoCJSON(const Value &v) {
  switch (v.getType()) {
  case ValueType::NIL:
    return cJSON_CreateNull();
  case ValueType::BYTE:
    return cJSON_CreateNumber(v.get<Byte_t>());
  case ValueType::BOOLEAN:
    return cJSON_CreateBool(v.get<Boolean_t>());
  case ValueType::STRING:
    return cJSON_CreateString(v.get<String_t>().c_str());
  case ValueType::INTEGER:
    return cJSON_CreateNumber(v.get<Integer_t>());
  case ValueType::UNSIGNED:
    return cJSON_CreateNumber(v.get<Unsigned_t>());
  case ValueType::FLOAT:
    return cJSON_CreateNumber(v.get<Float_t>());
  case ValueType::ARRAY: {
    cJSON *array = cJSON_CreateArray();
    auto &arr = v.get<Array_t>();
    for (auto &item : arr) {
      cJSON_AddItemToArray(array, ValuetoCJSON(item));
    }
    return array;
  }
  case ValueType::MAP: {
    cJSON *object = cJSON_CreateObject();
    auto &map = v.get<Map_t>();
    for (auto &[key, value] : map) {
      cJSON_AddItemToObject(object, key.c_str(), ValuetoCJSON(value));
    }
    return object;
  }
  }
  return cJSON_CreateNull();
}
Value Json::parse(const String_t &source) {
  cJSON *root = cJSON_Parse(source.c_str());
  Value result = cJSONtoValue(root);
  cJSON_free(root);
  return result;
}
std::string Json::stringify(const Value &object) {
  cJSON *root = ValuetoCJSON(object);
  auto res = cJSON_Print(root);
  cJSON_Minify(res);
  cJSON_free(root);
  std::string result = res;
  free(res);
  return result;
}