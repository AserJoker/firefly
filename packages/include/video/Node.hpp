#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <fmt/format.h>
#include <list>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
namespace firefly::video {

class Node : public core::Object {
public:
  enum AttrType { BOOLEAN, STRING, I32, U32, F32, F64, NIL };
  struct Attr {
    union {
      std::string *string;
      bool *boolean;
      int32_t *i32;
      uint32_t *u32;
      float *f32;
      double *f64;
    };
    AttrType type;
    Attr() { this->type = NIL; }
    Attr(std::string *string) {
      this->string = string;
      this->type = STRING;
    }
    Attr(bool *boolean) {
      this->boolean = boolean;
      this->type = BOOLEAN;
    }
    Attr(int32_t *i32) {
      this->i32 = i32;
      this->type = I32;
    }
    Attr(uint32_t *u32) {
      this->u32 = u32;
      this->type = U32;
    }
    Attr(float *f32) {
      this->f32 = f32;
      this->type = F32;
    }
    Attr(double *f64) {
      this->f64 = f64;
      this->type = F64;
    }
  };

  struct AttrValue {
    std::string string;
    bool boolean;
    int32_t i32;
    uint32_t u32;
    float f32;
    double f64;
    AttrType type;
    AttrValue() { this->type = NIL; }
    AttrValue(const std::string &string) {
      this->string = string;
      this->type = STRING;
    }
    AttrValue(bool boolean) {
      this->boolean = boolean;
      this->type = BOOLEAN;
    }
    AttrValue(int32_t i32) {
      this->i32 = i32;
      this->type = I32;
    }
    AttrValue(uint32_t u32) {
      this->u32 = u32;
      this->type = U32;
    }
    AttrValue(float f32) {
      this->f32 = f32;
      this->type = F32;
    }
    AttrValue(double f64) {
      this->f64 = f64;
      this->type = F64;
    }
    AttrValue(const Attr &attr) {
      this->type = attr.type;
      switch (attr.type) {
      case BOOLEAN:
        this->boolean = *attr.boolean;
        break;
      case STRING:
        this->string = *attr.string;
        break;
      case I32:
        this->i32 = *attr.i32;
        break;
      case U32:
        this->u32 = *attr.u32;
        break;
      case F32:
        this->f32 = *attr.f32;
        break;
      case F64:
        this->f64 = *attr.f64;
        break;
      case NIL:
        break;
      }
    }
    std::string toString() const {
      switch (type) {
      case BOOLEAN:
        return boolean ? "true" : "false";
      case STRING:
        return string;
      case I32:
        return fmt::format("{}", i32);
      case U32:
        return fmt::format("{}", u32);
      case F32:
        return fmt::format("{:g}", f32);
      case F64:
        return fmt::format("{:g}", f64);
      case NIL:
        return "nil";
      }
      return "";
    }
    double toNumber() const {
      switch (type) {
      case BOOLEAN:
        return boolean ? 1.0f : 0.0f;
      case STRING: {
        std::stringstream ss(string);
        double value;
        ss >> value;
        return value;
      }
      case I32:
        return i32;
      case U32:
        return u32;
      case F32:
        return f32;
      case F64:
        return f64;
      case NIL:
        return 0;
      }
      return .0f;
    }
    bool toBoolean() const {
      switch (type) {
      case BOOLEAN:
        return boolean;
      case STRING:
        return string == "true" || string == "TRUE";
      case I32:
        return i32 == 1;
      case U32:
        return u32 == 1;
      case F32:
        return f32 == 1;
      case F64:
        return f64 == 1;
      default:
        break;
      }
      return false;
    }
  };

  struct AttrBinding {
    std::string attr;
    Node *node;
  };

private:
  Node *_parent;
  std::list<core::AutoPtr<Node>> _children;
  std::unordered_map<std::string, core::AutoPtr<Node>> _indexedChildren;
  std::unordered_map<std::string, Attr> _attributes;
  std::unordered_map<std::string, std::vector<AttrBinding>> _bindings;
  std::vector<Node *> _bindingHosts;
  std::unordered_map<std::string, std::vector<std::string>> _attributeGroups;
  std::string _id;

  std::list<std::string> _attrGroups;
  std::string _attrGroup;
  uint32_t _changed;

protected:
  virtual void onAttrChange(const std::string &name);
  template <class T>
  inline void defineAttribute(const std::string &name, T *field) {
    _attributes[name] = field;
    auto pos = name.find_first_of('.');
    if (pos != std::string::npos) {
      auto groupName = name.substr(0, pos);
      auto &group = this->_attributeGroups[groupName];
      group.push_back(name);
    }
  }

public:
  Node();
  ~Node() override;
  void setId(const std::string &id);
  const std::string &getId() const;
  core::AutoPtr<Node> getChild(const std::string &id);
  const core::AutoPtr<Node> getChild(const std::string &id) const;
  void appendChild(core::AutoPtr<Node> node);
  void removeChild(core::AutoPtr<Node> node);
  core::AutoPtr<Node> getParent();
  std::list<core::AutoPtr<Node>> &getChildren();
  const AttrValue getAttribute(const std::string &name) const;
  void beginAttrGroup(const std::string &name);
  bool setAttribute(const std::string &name, const Attr &value);
  bool setAttribute(const std::string &name, const AttrValue &value);
  void endAttrGroup();
  void bindAttribute(const std::string name, core::AutoPtr<Node> host,
                     const std::string &source);
  virtual void onTick();
};
}; // namespace firefly::video