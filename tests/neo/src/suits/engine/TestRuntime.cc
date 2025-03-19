#include "script/engine/JSVirtualMachine.hpp"
#include "script/engine/JSRuntime.hpp"
#include <gtest/gtest.h>

class TestRuntime : public ::testing::Test {};
class MockParser : public JSParser {
public:
  MockParser(JSAllocator *allocator) : JSParser(allocator) {}
};
class MockGenerator : public JSCodeGenerator {
public:
  MockGenerator(JSAllocator *allocator) : JSCodeGenerator(allocator) {}
};
class MockVirtualMachine : public JSVirtualMachine {
public:
  MockVirtualMachine(JSAllocator *allocator) : JSVirtualMachine(allocator) {}
};
class MockLogger : public JSLogger {};
TEST_F(TestRuntime, args) {
  const char *mockArgs[] = {"test args"};
  auto runtime = new JSRuntime(1, (char **)&mockArgs[0]);
  auto args = runtime->getArgs();
  ASSERT_EQ(args.size(), 1);
  ASSERT_EQ(args[0], L"test args");
  delete runtime;
}
TEST_F(TestRuntime, customParser) {
  auto runtime = new JSRuntime(0, nullptr);
  auto parser = runtime->getAllocator()->create<MockParser>();
  runtime->setParser(parser);
  ASSERT_EQ(runtime->getParser(), parser);
  delete runtime;
}
TEST_F(TestRuntime, customGenerator) {
  auto runtime = new JSRuntime(0, nullptr);
  auto generator = runtime->getAllocator()->create<MockGenerator>();
  runtime->setGenerator(generator);
  ASSERT_EQ(runtime->getGenerator(), generator);
  delete runtime;
}