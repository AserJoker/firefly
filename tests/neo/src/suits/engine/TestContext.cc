#include "script/engine/JSArray.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSRuntime.hpp"
#include "script/engine/JSUndefinedType.hpp"
#include <gtest/gtest.h>
class TestContext : public testing::Test {};
TEST_F(TestContext, createNumber) {
  auto runtime = new JSRuntime(0, NULL);
  auto ctx = new JSContext(runtime);
  auto num = ctx->createNumber(123);
  ASSERT_EQ(ctx->checkedNumber(num), 123);
  delete ctx;
  delete runtime;
}
TEST_F(TestContext, createNativeFunction) {
  auto runtime = new JSRuntime(0, NULL);
  auto ctx = new JSContext(runtime);
  auto fn = ctx->createNativeFunction(
      [](JSContext *ctx, JSValue *self, std::vector<JSValue *> args)
          -> JSValue * { return ctx->createUndefined(); },
      L"test");
  ASSERT_TRUE(
      ctx->call(fn, ctx->createUndefined(), {})->isTypeof<JSUndefinedType>());
  delete ctx;
  delete runtime;
}
TEST_F(TestContext, setField) {
  auto runtime = new JSRuntime(0, NULL);
  auto ctx = new JSContext(runtime);
  auto obj = ctx->createObject(ctx->createNull());
  auto num = ctx->createNumber(123);
  ctx->setField(obj, ctx->createString(L"test"), num);
  auto val = ctx->getField(obj, ctx->createString(L"test"));
  ASSERT_EQ(ctx->checkedNumber(val), 123);
  delete ctx;
  delete runtime;
}
TEST_F(TestContext, setIndex) {
  auto runtime = new JSRuntime(0, NULL);
  auto ctx = new JSContext(runtime);
  auto arr = ctx->createArray();
  auto num = ctx->createNumber(123);
  ctx->setField(arr, ctx->createNumber(1), num);
  auto array = arr->getData()->cast<JSArray>();
  ASSERT_EQ(array->getLength(), 2);
  delete ctx;
  delete runtime;
}
TEST_F(TestContext, getFieldFromPrototype) {
  auto runtime = new JSRuntime(0, NULL);
  auto ctx = new JSContext(runtime);
  auto proto = ctx->createObject(ctx->createNull());
  auto num = ctx->createNumber(123);
  ctx->setField(proto, ctx->createString(L"num"), num);
  auto obj = ctx->createObject(proto);
  auto val = ctx->getField(obj, ctx->createString(L"num"));
  ASSERT_EQ(ctx->checkedNumber(val), 123);
  delete ctx;
  delete runtime;
}