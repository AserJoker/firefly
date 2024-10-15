#pragma once
#include "TestSuit.hpp"
#include "core/Map.hpp"
using namespace firefly;
class TestMap : public TestSuit<"TestMap", TestMap> {
private:
  void TEST_accessor(void) {
    core::Map<std::string, uint32_t> map;
    map["demo"] = 123;
    CU_ASSERT_EQUAL(map["demo"], 123);
    CU_ASSERT_EQUAL(map.size(), 1);
  }
  void TEST_erase(void) {
    core::Map<std::string, uint32_t> map;
    map["demo"] = 123;
    map.erase("demo");
    CU_ASSERT_EQUAL(map.size(), 0);
  }
  void TEST_keys(void) {
    core::Map<std::string, uint32_t> map;
    map["demo"] = 123;
    map["demo_2"] = 123;
    auto keys = map.keys();
    CU_ASSERT(keys.contains("demo"));
    CU_ASSERT(keys.contains("demo_2"));
  }
  void TEST_find(void) {
    core::Map<std::string, uint32_t> map;
    map["demo"] = 123;
    map["demo_2"] = 123;
    auto it = map.find("demo");
    CU_ASSERT_EQUAL(it->second, 123);
  }

public:
  TestMap() {
    defineTest<"accessor">(&TestMap::TEST_accessor);
    defineTest<"erase">(&TestMap::TEST_erase);
    defineTest<"keys">(&TestMap::TEST_keys);
    defineTest<"find">(&TestMap::TEST_find);
  }
};