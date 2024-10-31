#pragma once
#include "TestSuit.hpp"
#include "core/Array.hpp"
class TestArray : public TestSuit<"TestArray", TestArray> {
private:
  void TEST_accessor(void) {
    core::Array<core::Integer_t> array;
    array.pushBack({1, 2, 3, 4});
    CU_ASSERT_EQUAL_FATAL(array.size(), 4);
    CU_ASSERT_EQUAL_FATAL(array[0], 1);
  }

  void TEST_erase(void) {
    core::Array<core::Integer_t> array;
    array.pushBack({1, 1, 1, 4});
    array.erase(1);
    CU_ASSERT_EQUAL_FATAL(array.size(), 3);
    array.erase(1, true);
    CU_ASSERT_EQUAL_FATAL(array.size(), 1);
  }

  void TEST_indexOf(void) {
    core::Array<core::Integer_t> array{{0, 1, 2, 3, 4}};
    CU_ASSERT_EQUAL_FATAL(array.indexOf(1), 1);
    CU_ASSERT_EQUAL_FATAL(array.indexOf(-1), array.size());
  }

  void TEST_equal(void) {
    core::Array<core::Integer_t> array{{0, 1, 2, 3, 4}};
    core::Array<core::Integer_t> array2{{0, 1, 2, 3, 4}};
    CU_ASSERT_FATAL(array == array2);
  }

  void TEST_replace(void) {
    core::Array<core::Integer_t> array{{0, 1, 1, 1, 4}};
    array.replace(1, 2);
    CU_ASSERT_FATAL(array.contains(1));
    CU_ASSERT_FATAL(array.contains(2));
    array.replaceAll(1, 2);
    CU_ASSERT_FATAL(!array.contains(1));
  }

public:
  TestArray() {
    defineTest<"accessor">(&TestArray::TEST_accessor);
    defineTest<"erase">(&TestArray::TEST_erase);
    defineTest<"indexOf">(&TestArray::TEST_indexOf);
    defineTest<"equal">(&TestArray::TEST_equal);
    defineTest<"replace">(&TestArray::TEST_replace);
  }
};