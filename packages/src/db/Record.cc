#include "db/Record.hpp"
#include <any>
#include <exception>
#include <fmt/core.h>
#include <stdexcept>
using namespace firefly;
using namespace firefly::db;
Record::Record(const std::vector<core::AutoPtr<Field>> &fields,
               const std::unordered_map<std::string, std::any> &record)
    : _fields(fields) {
  for (auto &field : _fields) {
    auto &name = field->getName();
    if (record.contains(name)) {
      try {
        switch (field->getType()) {
        case Field::TYPE::STRING:
          _record[name] = std::any_cast<std::string>(record.at(name));
          break;
        case Field::TYPE::NUMBER:
          _record[name] = std::any_cast<float>(record.at(name));
          break;
        case Field::TYPE::BOOLEAN:
          _record[name] = std::any_cast<bool>(record.at(name));
          break;
        case Field::TYPE::O2M:
        case Field::TYPE::M2O:
        case Field::TYPE::O2O:
        case Field::TYPE::M2M:
        case Field::TYPE::ENUM:
          break;
        }
      } catch (std::exception &e) {
        throw std::runtime_error(
            fmt::format("Failed to initialize field '{}':{}", name, e.what()));
      }
    }
  };
}
const core::AutoPtr<Field> Record::getField(const std::string &name) const {
  for (auto &field : _fields) {
    if (field->getName() == name) {
      return field;
    }
  }
  return nullptr;
};
void Record::setField(const std::string &name, const std::string &data) {
  auto field = getField(name);
  if (!field) {
    throw std::runtime_error(fmt::format("Unknown string field '{}'", name));
  }
  if (field->isReadonly()) {
    throw std::runtime_error(
        fmt::format("Failed to set string field '{}',field is readonly", name));
  }
  if (field->getType() != Field::TYPE::STRING) {
    throw std::runtime_error(
        fmt::format("Field to set field '{}',cannot convert string to '{}'",
                    name, field->getTypeName()));
  }
  _record[name] = data;
}
void Record::setField(const std::string &name, const float &data) {
  auto field = getField(name);
  if (!field) {
    throw std::runtime_error(fmt::format("Unknown number field '{}'", name));
  }
  if (field->isReadonly()) {
    throw std::runtime_error(
        fmt::format("Failed to set number field '{}',field is readonly", name));
  }
  if (field->getType() != Field::TYPE::NUMBER) {
    throw std::runtime_error(
        fmt::format("Field to set field '{}',cannot convert number to '{}'",
                    name, field->getTypeName()));
  }
  _record[name] = data;
}
void Record::setField(const std::string &name, const bool &data) {
  auto field = getField(name);
  if (!field) {
    throw std::runtime_error(fmt::format("Unknown boolean field '{}'", name));
  }
  if (field->isReadonly()) {
    throw std::runtime_error(fmt::format(
        "Failed to set boolean field '{}',field is readonly", name));
  }
  if (field->getType() != Field::TYPE::BOOLEAN) {
    throw std::runtime_error(
        fmt::format("Field to set field '{}',cannot convert boolean to '{}'",
                    name, field->getTypeName()));
  }
  _record[name] = data;
}
void Record::clearField(const std::string &name) {
  auto field = getField(name);
  if (!field) {
    throw std::runtime_error(fmt::format("Unknown field '{}'", name));
  }
  if (field->isReadonly()) {
    throw std::runtime_error(
        fmt::format("Failed to clear field '{}',field is readonly", name));
  }
  if (field->isRequired()) {
    throw std::runtime_error(
        fmt::format("Failed to clear field '{}',field is required", name));
  }
  switch (field->getType()) {

  case Field::TYPE::STRING:
    _record[name] = "";
    break;
  case Field::TYPE::NUMBER:
    _record[name] = .0f;
    break;
  case Field::TYPE::BOOLEAN:
    _record[name] = false;
    break;
  case Field::TYPE::O2M:
  case Field::TYPE::M2O:
  case Field::TYPE::O2O:
  case Field::TYPE::M2M:
  case Field::TYPE::ENUM:
    break;
  }
}
void Record::getField(const std::string &name, std::string *data) {
  auto field = getField(name);
  if (!field) {
    throw std::runtime_error(fmt::format("Unknown field '{}'", name));
  }
  if (field->getType() != Field::TYPE::STRING) {
    throw std::runtime_error(
        fmt::format("Field to set field '{}',cannot convert '{}' to STRING",
                    name, field->getTypeName()));
  }
  if (_record.contains(name)) {
    *data = std::any_cast<std::string>(_record.at(name));
  }
  *data = "";
}
void Record::getField(const std::string &name, float *data) {
  auto field = getField(name);
  if (!field) {
    throw std::runtime_error(fmt::format("Unknown field '{}'", name));
  }
  if (field->getType() != Field::TYPE::STRING) {
    throw std::runtime_error(
        fmt::format("Field to set field '{}',cannot convert '{}' to NUMBER",
                    name, field->getTypeName()));
  }
  if (_record.contains(name)) {
    *data = std::any_cast<float>(_record.at(name));
  }
  *data = .0f;
}
void Record::getField(const std::string &name, bool *data) {
  auto field = getField(name);
  if (!field) {
    throw std::runtime_error(fmt::format("Unknown field '{}'", name));
  }
  if (field->getType() != Field::TYPE::STRING) {
    throw std::runtime_error(
        fmt::format("Field to set field '{}',cannot convert '{}' to BOOLEAN",
                    name, field->getTypeName()));
  }
  if (_record.contains(name)) {
    *data = std::any_cast<bool>(_record.at(name));
  }
  *data = false;
}