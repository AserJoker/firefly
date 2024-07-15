#include "db/Database.hpp"
#include "core/AutoPtr.hpp"
#include "db/Field.hpp"
#include "db/Field_Complex.hpp"
#include "db/Field_Enum.hpp"
#include "db/Table.hpp"
#include "runtime/Resource.hpp"
#include <exception>
#include <fmt/core.h>
#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/valid.h>
#include <libxml/xmlstring.h>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>
using namespace firefly;
using namespace firefly::db;
void Database::loadTable(core::AutoPtr<runtime::Resource> resource) {
  auto buffer = resource->read();
  xmlDocPtr doc = xmlParseMemory((char *)buffer->getData(), buffer->getSize());
  if (!doc) {
    throw std::runtime_error(xmlGetLastError()->message);
  }
  try {
    auto root = xmlDocGetRootElement(doc);
    if (xmlStrcmp(root->name, (xmlChar *)"table") != 0) {
      throw std::runtime_error(fmt::format(
          "Invalid table format,root node is '{}'", (const char *)root->name));
    }
    auto id = xmlGetProp(root, (xmlChar *)"id");
    if (!id) {
      throw std::runtime_error("Invalid table format,table id is required");
    }
    core::AutoPtr<Table> table;
    for (auto &tt : _tables) {
      if (tt->getId() == (const char *)id) {
        table = tt;
        break;
      }
    }
    if (!table) {
      throw std::runtime_error(
          fmt::format("Cannot find table definition '{}'", (const char *)id));
    }
    auto &fields = table->getFields();
    auto node = root->children;
    while (node) {
      if (xmlStrcmp(node->name, (const xmlChar *)"record") == 0) {
        std::unordered_map<std::string, std::any> record;
        auto fnode = node->children;
        while (fnode) {
          if (xmlStrcmp(fnode->name, (const xmlChar *)"field") == 0) {
            auto name =
                (const char *)xmlGetProp(fnode, (const xmlChar *)"name");
            if (!name) {
              throw std::runtime_error("Field name is required");
            }
            auto value =
                (const char *)xmlGetProp(fnode, (const xmlChar *)"value");
            core::AutoPtr<Field> field;
            for (auto &ff : fields) {
              if (ff->getName() == name) {
                field = ff;
                break;
              }
            }
            if (!field) {
              throw std::runtime_error(fmt::format("Unknown field '{}'", name));
            }
            if (field->isArray()) {
              record[name] = std::string(value);
            } else {
              switch (field->getType()) {
              case Field::TYPE::O2M:
              case Field::TYPE::M2O:
              case Field::TYPE::O2O:
              case Field::TYPE::M2M:
                throw std::runtime_error(
                    fmt::format("Cannot write complex field,'{}'", name));
              case Field::TYPE::STRING:
                record[name] = std::string(value);
                break;
              case Field::TYPE::INTEGER: {
                int32_t val;
                std::stringstream ss(value);
                ss >> val;
                record[name] = val;
                break;
              }
              case Field::TYPE::NUMBER: {
                double val;
                std::stringstream ss(value);
                ss >> val;
                record[name] = val;
                break;
              }
              case Field::TYPE::BOOLEAN: {
                record[name] = std::string(value) == "true";
                break;
              }
              case Field::TYPE::ENUM:
                record[name] = std::string(value);
                break;
              }
            }
          }
          fnode = fnode->next;
        }
        table->addRecord(record);
      }
      node = node->next;
    }
  } catch (std::exception &e) {
    xmlFreeDoc(doc);
    throw std::runtime_error(e.what());
  }
  xmlFreeDoc(doc);
}
Database::Database() {
  initFieldTable();
  initTableTable();
  _tableTable->addRecord({
      {"id", std::string("firefly.table")},
      {"name", std::string("table")},
      {"namespace", std::string("firefly")},
  });
  _tableTable->addRecord({
      {"id", std::string("firefly.field")},
      {"name", std::string("field")},
      {"namespace", std::string("firefly")},
  });
  _tableField->addRecord({
      {"id", std::string("firefly.field.id")},
      {"name", std::string("id")},
      {"namespace", std::string("firefly.field")},
      {"type", std::string("STRING")},
      {"readonly", true},
      {"required", true},
  });
  _tableField->addRecord({
      {"id", std::string("firefly.field.name")},
      {"name", std::string("name")},
      {"namespace", std::string("firefly.field")},
      {"type", std::string("STRING")},
      {"readonly", true},
      {"required", true},
  });
  _tableField->addRecord({
      {"id", std::string("firefly.field.namespace")},
      {"name", std::string("namespace")},
      {"namespace", std::string("firefly.field")},
      {"type", std::string("STRING")},
      {"readonly", true},
      {"required", true},
  });
  _tableField->addRecord({
      {"id", std::string("firefly.field.type")},
      {"name", std::string("type")},
      {"namespace", std::string("firefly.field")},
      {"type", std::string("ENUM")},
      {"enums",
       std::string("O2M,M2O,O2O,M2M,STRING,INTEGER,NUMBER,BOOLEAN,ENUM")},
      {"readonly", true},
      {"required", true},
  });
  _tableField->addRecord({
      {"id", std::string("firefly.field.readonly")},
      {"name", std::string("readonly")},
      {"namespace", std::string("firefly.field")},
      {"type", std::string("BOOLEAN")},
  });
  _tableField->addRecord({
      {"id", std::string("firefly.field.required")},
      {"name", std::string("required")},
      {"namespace", std::string("firefly.field")},
      {"type", std::string("BOOLEAN")},
  });
  _tableField->addRecord({
      {"id", std::string("firefly.field.array")},
      {"name", std::string("required")},
      {"namespace", std::string("firefly.field")},
      {"type", std::string("BOOLEAN")},
  });
  _tableField->addRecord({
      {"id", std::string("firefly.field.relationTable")},
      {"name", std::string("relationTable")},
      {"namespace", std::string("firefly.field")},
      {"type", std::string("STRING")},
  });
  _tableField->addRecord({
      {"id", std::string("firefly.field.relationFields")},
      {"name", std::string("relationFields")},
      {"namespace", std::string("firefly.field")},
      {"type", std::string("STRING")},
      {"array", true},
  });
  _tableField->addRecord({
      {"id", std::string("firefly.field.referenceFields")},
      {"name", std::string("referenceFields")},
      {"namespace", std::string("firefly.field")},
      {"type", std::string("STRING")},
      {"array", true},
  });
  _tableField->addRecord({
      {"id", std::string("firefly.field.enums")},
      {"name", std::string("enums")},
      {"namespace", std::string("firefly.field")},
      {"type", std::string("STRING")},
      {"array", true},
  });

  _tableField->addRecord({
      {"id", std::string("firefly.table.id")},
      {"name", std::string("id")},
      {"namespace", std::string("firefly.table")},
      {"type", std::string("STRING")},
      {"readonly", true},
      {"required", true},
  });
  _tableField->addRecord({
      {"id", std::string("firefly.table.name")},
      {"name", std::string("name")},
      {"namespace", std::string("firefly.table")},
      {"type", std::string("STRING")},
      {"readonly", true},
      {"required", true},
  });
  _tableField->addRecord({
      {"id", std::string("firefly.table.namespace")},
      {"name", std::string("namespace")},
      {"namespace", std::string("firefly.table")},
      {"type", std::string("STRING")},
      {"readonly", true},
      {"required", true},
  });
}
void Database::initFieldTable() {
  std::vector<core::AutoPtr<Field>> fields;
  fields.push_back(
      new Field("id", "firefly.field", Field::TYPE::STRING, true, true));
  fields.push_back(
      new Field("name", "firefly.field", Field::TYPE::STRING, true, true));
  fields.push_back(
      new Field("namespace", "firefly.field", Field::TYPE::STRING, true, true));
  fields.push_back(new Field_Enum("type", "firefly.field",
                                  {"O2M", "M2O", "O2O", "M2M", "STRING",
                                   "INTEGER", "NUMBER", "BOOLEAN", "ENUM"},
                                  true, true));
  fields.push_back(new Field("readonly", "firefly.field", Field::TYPE::BOOLEAN,
                             true, false, false));
  fields.push_back(new Field("required", "firefly.field", Field::TYPE::BOOLEAN,
                             true, false, false));
  fields.push_back(new Field("array", "firefly.field", Field::TYPE::BOOLEAN,
                             true, false, false));
  fields.push_back(new Field("relationTable", "firefly.field",
                             Field::TYPE::STRING, true, false, false));
  fields.push_back(new Field("relationFields", "firefly.field",
                             Field::TYPE::STRING, true, false, true));
  fields.push_back(new Field("referenceFields", "firefly.field",
                             Field::TYPE::STRING, true, false, true));
  fields.push_back(new Field("enums", "firefly.field", Field::TYPE::STRING,
                             true, false, true));
  _tableField = new Table("field", "firefly", fields);
  _tables.push_back(_tableField);
}
void Database::initTableTable() {
  std::vector<core::AutoPtr<Field>> fields;
  fields.push_back(
      new Field("id", "firefly.table", Field::TYPE::STRING, true, true));
  fields.push_back(
      new Field("name", "firefly.table", Field::TYPE::STRING, true, true));
  fields.push_back(
      new Field("namespace", "firefly.table", Field::TYPE::STRING, true, true));
  _tableTable = new Table("table", "firefly", fields);
  _tables.push_back(_tableTable);
}
void Database::print() {
  for (auto &table : _tables) {
    std::cout << "### " << table->getId() << std::endl;
    auto &fields = table->getFields();
    std::cout << "|";
    for (auto &field : fields) {
      std::cout << field->getName() << "(" << field->getTypeName() << ")"
                << "|";
    }
    std::cout << std::endl;
    std::cout << "|";
    for (auto &field : fields) {
      std::cout << "-" << "|";
    }
    std::cout << std::endl;
    for (auto &record : table->getRecords()) {

      std::cout << "|";
      for (auto &field : fields) {
        if (record->isNil(field->getName())) {
          std::cout << "nil";
        } else if (field->isArray()) {
          switch (field->getType()) {
          case Field::TYPE::O2M:
          case Field::TYPE::M2O:
          case Field::TYPE::O2O:
          case Field::TYPE::M2M:
            break;
          case Field::TYPE::STRING: {
            auto data = record->getStringArrayField(field->getName());
            for (auto &d : data) {
              std::cout << d << ",";
            }
            break;
          }
          case Field::TYPE::INTEGER: {
            auto data = record->getIntegerArrayField(field->getName());
            for (auto &d : data) {
              std::cout << d << ",";
            }
            break;
          }
          case Field::TYPE::NUMBER: {
            auto data = record->getNumberArrayField(field->getName());
            for (auto &d : data) {
              std::cout << d << ",";
            }
            break;
          }
          case Field::TYPE::BOOLEAN: {
            auto data = record->getBooleanArrayField(field->getName());
            for (auto d : data) {
              std::cout << d << ",";
            }
            break;
          }
          case Field::TYPE::ENUM: {
            auto data = record->getEnumArrayField(field->getName());
            for (auto &d : data) {
              std::cout << d << ",";
            }
            break;
          }
          }
        } else {
          switch (field->getType()) {
          case Field::TYPE::O2M:
          case Field::TYPE::M2O:
          case Field::TYPE::O2O:
          case Field::TYPE::M2M:
            break;
          case Field::TYPE::STRING:
            std::cout << record->getStringField(field->getName());
            break;
          case Field::TYPE::INTEGER:
            std::cout << record->getIntegerField(field->getName());
            break;
          case Field::TYPE::NUMBER:
            std::cout << record->getNumberField(field->getName());
            break;
          case Field::TYPE::BOOLEAN:
            std::cout << record->getBooleanField(field->getName());
            break;
          case Field::TYPE::ENUM:
            std::cout << record->getEnumField(field->getName());
            break;
          }
        }
        std::cout << "|";
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
}
void Database::reload() {
  std::vector<core::AutoPtr<Table>> tables;
  for (auto &meta : _tableTable->getRecords()) {
    auto id = meta->getStringField("id");
    core::AutoPtr<Table> t;
    for (auto &tt : _tables) {
      if (tt->getId() == id) {
        t = tt;
        break;
      }
    }
    if (!t) {
      std::vector<core::AutoPtr<Field>> fields;
      for (auto &fmeta : _tableField->getRecords()) {
        if (fmeta->getStringField("namespace") == id) {
          std::string type_s = fmeta->getEnumField("type");
          Field::TYPE type;
          if (type_s == "O2M") {
            type = Field::TYPE::O2M;
          } else if (type_s == "M2M") {
            type = Field::TYPE::M2M;
          } else if (type_s == "M2O") {
            type = Field::TYPE::M2O;
          } else if (type_s == "O2O") {
            type = Field::TYPE::O2O;
          } else if (type_s == "ENUM") {
            type = Field::TYPE::ENUM;
          } else if (type_s == "STRING") {
            type = Field::TYPE::STRING;
          } else if (type_s == "NUMBER") {
            type = Field::TYPE::NUMBER;
          } else if (type_s == "INTEGER") {
            type = Field::TYPE::INTEGER;
          } else if (type_s == "BOOLEAN") {
            type = Field::TYPE::BOOLEAN;
          }
          if (type == Field::TYPE::O2M || type == Field::TYPE::M2M ||
              type == Field::TYPE::M2O || type == Field::TYPE::O2O) {
            fields.push_back(
                new Field_Complex(fmeta->getStringField("name"),
                                  fmeta->getStringField("namespace"),
                                  (Field_Complex::COMPLEX_TYPE)type,
                                  fmeta->getStringField("relationTable"),
                                  fmeta->getStringArrayField("relationFields"),
                                  fmeta->getStringArrayField("referenceFields"),
                                  fmeta->getBooleanField("readonly"),
                                  fmeta->getBooleanField("required")));
          } else if (type == Field::TYPE::ENUM) {
            fields.push_back(new Field_Enum(fmeta->getStringField("name"),
                                            fmeta->getStringField("namespace"),
                                            fmeta->getStringArrayField("enums"),
                                            fmeta->getBooleanField("readonly"),
                                            fmeta->getBooleanField("required"),
                                            fmeta->getBooleanField("array")));
          } else {
            fields.push_back(new Field(fmeta->getStringField("name"),
                                       fmeta->getStringField("namespace"), type,
                                       fmeta->getBooleanField("readonly"),
                                       fmeta->getBooleanField("required"),
                                       fmeta->getBooleanField("array")));
          }
        }
      }
      tables.push_back(new Table(meta->getStringField("name"),
                                 meta->getStringField("namespace"), fields));
    } else {
      tables.push_back(t);
    }
  }
  _tables = tables;
}