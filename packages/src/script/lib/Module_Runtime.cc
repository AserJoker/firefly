#include "script/lib/Module_Runtime.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/File.hpp"
#include "core/Properties.hpp"
#include "core/Singleton.hpp"
#include "exception/ValidateException.hpp"
#include "runtime/Application.hpp"
#include "runtime/Resource.hpp"
#include "script/helper/Trait_Buffer.hpp"
#include "script/helper/Trait_Properties.hpp"
#include <filesystem>

using namespace firefly;
using namespace firefly::script;
using exception::ValidateException;
void Module_Runtime::open(core::AutoPtr<Script> ctx) {
  auto exports = ctx->createValue()->setObject(ctx);
  exports->setFunctionField(ctx, setWindowTitle);
  exports->setFunctionField(ctx, getWindowTitle);
  exports->setFunctionField(ctx, setWindowSize);
  exports->setFunctionField(ctx, getWindowSize);
  exports->setFunctionField(ctx, loadConfig);
  exports->setFunctionField(ctx, saveConfig);
  exports->setFunctionField(ctx, save);
  exports->setFunctionField(ctx, load);
  exports->setFunctionField(ctx, getSaveInfo);
  exports->setFunctionField(ctx, createProperties);
  ctx->registerModule("runtime", exports);
}
FUNC_DEF(Module_Runtime::setWindowTitle) {
  auto title = args[0]->toString(ctx);
  auto app = core::Singleton<runtime::Application>::instance();
  app->getWindow()->setTitle(title);
  return {};
}
FUNC_DEF(Module_Runtime::getWindowTitle) {
  auto app = core::Singleton<runtime::Application>::instance();
  auto title = app->getWindow()->getTitle();
  return {ctx->createValue()->setString(ctx, title)};
}
FUNC_DEF(Module_Runtime::setWindowSize) {
  uint32_t width = (uint32_t)args[0]->toNumber(ctx);
  uint32_t height = (uint32_t)args[1]->toNumber(ctx);
  auto app = core::Singleton<runtime::Application>::instance();
  app->getWindow()->setSize({width, height});
  return {};
}
FUNC_DEF(Module_Runtime::getWindowSize) {
  auto app = core::Singleton<runtime::Application>::instance();
  auto size = app->getWindow()->getSize();
  return {ctx->createValue()->setNumber(ctx, size.x),
          ctx->createValue()->setNumber(ctx, size.y)};
}

FUNC_DEF(Module_Runtime::saveConfig) {
  auto module = args[0]->toString(ctx);
  auto filename = args[1]->toString(ctx);
  auto config = args[2]->getOpaque().cast<core::Properties>();
  auto data = config->store();
  std::filesystem::path configPath = "./config";
  configPath = configPath.append(module);
  if (!std::filesystem::exists(configPath)) {
    std::filesystem::create_directories(configPath);
  }
  configPath = configPath.append(filename + ".cfg");
  core::AutoPtr file =
      new runtime::ResourceTrait<core::File>(configPath.string());
  core::AutoPtr buf =
      new core::Buffer((uint32_t)data.size(), (char *)data.c_str());
  file->write(buf);
  return {};
}
FUNC_DEF(Module_Runtime::loadConfig) {
  auto module = args[0]->toString(ctx);
  auto filename = args[1]->toString(ctx);
  std::filesystem::path configPath = "./config";
  configPath = configPath.append(module);
  if (!std::filesystem::exists(configPath)) {
    return {};
  }
  configPath = configPath.append(filename + ".cfg");
  if (!std::filesystem::exists(configPath)) {
    return {};
  }
  core::AutoPtr file =
      new runtime::ResourceTrait<core::File>(configPath.string());
  auto buf = file->read();
  return {Trait_Properties::create(ctx, new core::Properties(buf))};
}
FUNC_DEF(Module_Runtime::save) {
  auto name = args[0]->toString(ctx);
  auto file = args[1]->toString(ctx);
  std::filesystem::path savePath = "./saves";
  savePath = savePath.append(name);
  if (std::filesystem::exists(savePath)) {
    std::filesystem::create_directories(savePath);
  }
  savePath = savePath.append(file + ".data");
  core::AutoPtr resource =
      new runtime::ResourceTrait<core::File>(savePath.string());
  resource->write(args[2]->getOpaque().cast<core::Buffer>());
  return {};
}
FUNC_DEF(Module_Runtime::load) {
  auto name = args[0]->toString(ctx);
  auto file = args[1]->toString(ctx);
  std::filesystem::path savePath = "./saves";
  savePath = savePath.append(name);
  savePath = savePath.append(file + ".data");
  if (!std::filesystem::exists(savePath)) {
    return {};
  }
  core::AutoPtr resource =
      new runtime::ResourceTrait<core::File>(savePath.string());
  auto buffer = resource->read();
  return {Trait_Buffer::create(ctx, buffer)};
}
FUNC_DEF(Module_Runtime::createProperties) {
  return {Trait_Properties::create(ctx, new core::Properties())};
}
FUNC_DEF(Module_Runtime::getSaveInfo) {
  auto arr = ctx->createValue()->setArray(ctx);
  std::filesystem::path savePath = "./saves";
  auto index = 0;
  for (auto &item : std::filesystem::directory_iterator(savePath)) {
    if (item.is_directory()) {
      arr->setIndex(ctx, index++,
                    ctx->createValue(item.path().filename().string()));
    }
  }
  return {arr};
}