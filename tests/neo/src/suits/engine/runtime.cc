#include "script/neo.hpp"
#include <gtest/gtest.h>
class TestRuntime : public ::testing::Test {};
TEST_F(TestRuntime, args) {
  const char *mockArgs[] = {"test args"};
  auto runtime = new neo::JSRuntime(1, (char **)&mockArgs[0]);
  auto args = runtime->getArgs();
  ASSERT_EQ(args.size(), 1);
  ASSERT_EQ(args[0], L"test args");
  delete runtime;
}