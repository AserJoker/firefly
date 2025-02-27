#include "script/neo.hpp"
#include <gtest/gtest.h>
class TestRuntime : public ::testing::Test {};
class MockParser : public neo::JSParser {
public:
  MockParser(neo::JSAllocator *allocator) : neo::JSParser(allocator) {}
};
class MockGenerator : public neo::JSCodeGenerator {
public:
  MockGenerator(neo::JSAllocator *allocator)
      : neo::JSCodeGenerator(allocator) {}
};
class MockVirtualMachine : public neo::JSVirtualMachine {
public:
  MockVirtualMachine(neo::JSAllocator *allocator)
      : neo::JSVirtualMachine(allocator) {}
};
class MockLogger : public neo::JSLogger {};
TEST_F(TestRuntime, args) {
  const char *mockArgs[] = {"test args"};
  auto runtime = new neo::JSRuntime(1, (char **)&mockArgs[0]);
  auto args = runtime->getArgs();
  ASSERT_EQ(args.size(), 1);
  ASSERT_EQ(args[0], L"test args");
  delete runtime;
}
TEST_F(TestRuntime, customParser) {
  auto runtime = new neo::JSRuntime(0, nullptr);
  auto parser = runtime->getAllocator()->create<MockParser>();
  runtime->setParser(parser);
  ASSERT_EQ(runtime->getParser(), parser);
  delete runtime;
}
TEST_F(TestRuntime, customGenerator) {
  auto runtime = new neo::JSRuntime(0, nullptr);
  auto generator = runtime->getAllocator()->create<MockGenerator>();
  runtime->setGenerator(generator);
  ASSERT_EQ(runtime->getGenerator(), generator);
  delete runtime;
}