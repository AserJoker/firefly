#include "script/engine/JSCallableType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSExceptionType.hpp"
#include "script/engine/JSNullType.hpp"
#include "script/engine/JSObjectType.hpp"
#include <gtest/gtest.h>
#include <string>
class TestObject : public testing::Test {
protected:
  JSRuntime *runtime;
  JSContext *ctx;
  void SetUp() override {
    runtime = new JSRuntime(0, NULL);
    ctx = new JSContext(runtime);
  }
  void TearDown() override {
    delete ctx;
    delete runtime;
    ASSERT_TRUE(JSRef::registry().empty());
  }
};

TEST_F(TestObject, constructor) {
  auto Object = ctx->getField(ctx->getGlobal(), ctx->createString(L"Object"));
  ASSERT_TRUE(Object->isTypeof<JSCallableType>());
  auto Function =
      ctx->getField(ctx->getGlobal(), ctx->createString(L"Function"));
  ASSERT_TRUE(Function->isTypeof<JSCallableType>());
  ASSERT_TRUE(ctx->checkedBoolean(
      ctx->isEqual(ctx->getField(Object, ctx->createString(L"name")),
                   ctx->createString(L"Object"))));
  ASSERT_TRUE(ctx->checkedBoolean(ctx->isEqual(
      ctx->getField(Object, ctx->createString(L"constructor")), Function)));
  auto prototype = ctx->getField(Object, ctx->createString(L"prototype"));
  ASSERT_TRUE(prototype && prototype->isTypeof<JSObjectType>());
  ASSERT_TRUE(ctx->checkedBoolean(ctx->isEqual(
      ctx->getField(prototype, ctx->createString(L"constructor")), Object)));
  auto proto = ctx->getPrototypeOf(prototype);
  ASSERT_TRUE(proto && proto->isTypeof<JSNullType>());
}

TEST_F(TestObject, prototype) {
  auto Object = ctx->getField(ctx->getGlobal(), ctx->createString(L"Object"));
  ASSERT_TRUE(Object->isTypeof<JSCallableType>());
  auto Function =
      ctx->getField(ctx->getGlobal(), ctx->createString(L"Function"));
  ASSERT_TRUE(Function->isTypeof<JSCallableType>());
  auto prototype = ctx->getPrototypeOf(Object);
  ASSERT_TRUE(ctx->checkedBoolean(ctx->isEqual(
      prototype, ctx->getField(Function, ctx->createString(L"prototype")))));
}

TEST_F(TestObject, toString) {
  auto object = ctx->createObject();
  std::wstring name = object->getType()->getTypeName();
  ASSERT_TRUE(object && object->isTypeof<JSObjectType>());
  ASSERT_EQ(ctx->checkedString(ctx->toString(object)), L"{}");
  object = ctx->createObject(ctx->createNull());
  auto str = ctx->toString(object);
  ASSERT_TRUE(!str->isTypeof<JSExceptionType>());
  ASSERT_EQ(ctx->checkedString(str), L"[Object: null prototype] {}");
  object = ctx->createObject();
  auto Function =
      ctx->getField(ctx->getGlobal(), ctx->createString(L"Function"));
  ctx->setConstructor(object, Function);

  ASSERT_EQ(ctx->checkedString(ctx->toString(object)), L"Function {}");
}