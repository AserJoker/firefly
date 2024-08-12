#include "script/lib/Module_Database.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "database/Database.hpp"
#include "database/Metadata.hpp"
#include "database/Record.hpp"
#include "database/Value.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
#include <unordered_map>
#include <vector>
using namespace firefly;
using namespace firefly::script;
void Module_Database::open(core::AutoPtr<Script> ctx) {
  auto exports = ctx->createValue()
                     ->setObject(ctx)
                     ->setFunctionField(ctx, queryOne)
                     ->setFunctionField(ctx, queryList);
  ctx->registerModule("database", exports);
}
static core::AutoPtr<database::Record>
createQuery(core::AutoPtr<Script> ctx,
            core::AutoPtr<database::Metadata> metadata,
            core::AutoPtr<Value> source) {
  std::unordered_map<std::string, core::AutoPtr<database::Value>> query;
  auto keys = source->getKeys(ctx);
  for (auto &key : keys) {
    auto &meta = metadata->getField(key);
    if (meta.isArray()) {
      switch (meta.getType()) {
      case database::Field::TYPE::STRING: {
        std::vector<std::string> value;
        auto array = source->getField(ctx, key);
        for (auto i = 0; i < array->getLength(ctx); i++) {
          value.push_back(array->getIndex(ctx, i)->toString(ctx));
        }
        query[meta.getName()] = new database::Value(value);
      } break;
      case database::Field::TYPE::INTEGER: {
        std::vector<int32_t> value;
        auto array = source->getField(ctx, key);
        for (auto i = 0; i < array->getLength(ctx); i++) {
          value.push_back((int32_t)array->getIndex(ctx, i)->toNumber(ctx));
        }
        query[meta.getName()] = new database::Value(value);
      } break;
      case database::Field::TYPE::NUMBER: {
        std::vector<double> value;
        auto array = source->getField(ctx, key);
        for (auto i = 0; i < array->getLength(ctx); i++) {
          value.push_back(array->getIndex(ctx, i)->toNumber(ctx));
        }
        query[meta.getName()] = new database::Value(value);
      } break;
      case database::Field::TYPE::BOOLEAN: {
        std::vector<bool> value;
        auto array = source->getField(ctx, key);
        for (auto i = 0; i < array->getLength(ctx); i++) {
          value.push_back(array->getIndex(ctx, i)->toBoolean(ctx));
        }
        query[meta.getName()] = new database::Value(value);
      } break;
      }
    } else {
      switch (meta.getType()) {
      case database::Field::TYPE::STRING:
        query[meta.getName()] =
            new database::Value(source->getField(ctx, key)->toString(ctx));
        break;
      case database::Field::TYPE::INTEGER:
        query[meta.getName()] = new database::Value(
            (int32_t)source->getField(ctx, key)->toNumber(ctx));
        break;
      case database::Field::TYPE::NUMBER:
        query[meta.getName()] =
            new database::Value(source->getField(ctx, key)->toNumber(ctx));
        break;
      case database::Field::TYPE::BOOLEAN:
        query[meta.getName()] =
            new database::Value(source->getField(ctx, key)->toBoolean(ctx));
        break;
      }
    }
  }
  return new database::Record(metadata, query);
}

static core::AutoPtr<Value>
createRecord(core::AutoPtr<Script> ctx, core::AutoPtr<database::Metadata> meta,
             core::AutoPtr<database::Record> source) {
  auto result = ctx->createValue()->setObject(ctx);
  for (auto &field : meta->getFields()) {
    if (source->hasField(field.getName())) {
      if (field.isArray()) {
        auto arr = ctx->createValue()->setArray(ctx);
        switch (field.getType()) {
        case database::Field::TYPE::STRING: {
          auto index = 0;
          auto res = source->getField(field.getName())->getStringArrayValue();
          for (auto &item : res) {
            arr->setIndex(ctx, index++, createString(ctx, item));
          }
        } break;
        case database::Field::TYPE::INTEGER: {
          auto index = 0;
          auto res = source->getField(field.getName())->getIntegerArrayValue();
          for (auto &item : res) {
            arr->setIndex(ctx, index++, createNumber(ctx, item));
          }
        } break;
        case database::Field::TYPE::NUMBER: {
          auto index = 0;
          auto res = source->getField(field.getName())->getNumberArrayValue();
          for (auto &item : res) {
            arr->setIndex(ctx, index++, createNumber(ctx, item));
          }
        } break;
        case database::Field::TYPE::BOOLEAN: {
          auto index = 0;
          auto res = source->getField(field.getName())->getBooleanArrayValue();
          for (auto item : res) {
            arr->setIndex(ctx, index++, createBoolean(ctx, item));
          }
        } break;
        }
        result->setField(ctx, field.getName(), arr);
      } else {
        switch (field.getType()) {
        case database::Field::TYPE::STRING:
          result->setField(
              ctx, field.getName(),
              createString(
                  ctx, source->getField(field.getName())->getStringValue()));
          break;
        case database::Field::TYPE::INTEGER:
          result->setField(
              ctx, field.getName(),
              createNumber(
                  ctx, source->getField(field.getName())->getIntegerValue()));
          break;
        case database::Field::TYPE::NUMBER:
          result->setField(
              ctx, field.getName(),
              createNumber(
                  ctx, source->getField(field.getName())->getNumberValue()));
          break;
        case database::Field::TYPE::BOOLEAN:
          result->setField(
              ctx, field.getName(),
              createBoolean(
                  ctx, source->getField(field.getName())->getBooleanValue()));
          break;
        }
      }
    }
  }
  return result;
}

FUNC_DEF(Module_Database::queryOne) {
  auto db = core::Singleton<database::Database>::instance();
  auto tableName = args[0]->toString(ctx);
  auto table = db->getTable(tableName);
  auto query = createQuery(ctx, table->getMetadata(), args[1]);
  auto result = table->queryOne(query);
  return {createRecord(ctx, table->getMetadata(), result)};
}

FUNC_DEF(Module_Database::queryList) {
  auto db = core::Singleton<database::Database>::instance();
  auto tableName = args[0]->toString(ctx);
  auto table = db->getTable(tableName);
  auto query = createQuery(ctx, table->getMetadata(), args[1]);
  auto result = table->queryList(query);
  auto index = 0;
  auto array = ctx->createValue()->setArray(ctx);
  for (auto &record : result) {
    array->setIndex(ctx, index++,
                    createRecord(ctx, table->getMetadata(), record));
  }
  return {array};
}