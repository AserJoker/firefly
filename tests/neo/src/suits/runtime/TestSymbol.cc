#include "script/engine/JSCallableType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSRuntime.hpp"
#include <gtest/gtest.h>
class TestSymbol : public testing::Test {
protected:
  JSRuntime *runtime;
  JSContext *ctx;
  void SetUp() override {
    runtime = new JSRuntime(0, nullptr);
    ctx = new JSContext(runtime);
  }
  void TearDown() override {
    delete ctx;
    delete runtime;
  }
};
TEST_F(TestSymbol, constructor) {
  auto Symbol = ctx->getField(ctx->getGlobal(), ctx->createString(L"Symbol"));
  ASSERT_TRUE(Symbol->isTypeof<JSCallableType>());
}