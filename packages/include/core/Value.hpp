#pragma once
#include <any>
#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>
namespace firefly::core {
class Value {
public:
  enum class TYPE { NIL, NUMBER, STRING, BOOLEAN, ARRAY, OBJECT };

  using Array = std::vector<Value>;
  using Object = std::unordered_map<std::string, Value>;

private:
  TYPE _type;
  std::any _store;
  bool _bind;

  std::function<void(double)> _setNumber;
  std::function<double()> _getNumber;

public:
  Value();

  Value(const std::nullptr_t &);

  Value(const std::string &value);

  Value(const char *value);

  Value(double value);

  Value(bool value);

  Value(const Array &value);

  Value(const Object &value);

  Value(const Value &another);

  Value(Value &&another);

  virtual ~Value();

  const TYPE &getType() const;

  bool isBind() const;

  void setNil();

  bool isNil() const;

  void setArray();

  bool isArray() const;

  void setObject();

  bool isObject() const;

  void setString(const std::string &value);

  const std::string &getString() const;

  const std::string &checkString() const;

  void setNumber(double value);

  double getNumber() const;

  double checkNumber() const;

  void setBoolean(bool value);

  bool getBoolean() const;

  bool checkBoolean() const;

  const Array &getArray() const;

  Array &getArray();

  const Array &checkArray() const;

  const Object &getObject() const;

  Object &getObject();

  const Object &checkObject() const;

  Value &operator=(const Value &another);

  bool operator==(const Value &another) const;

public:
  static Value bindString(std::string *src);

  template <class T>
  static Value bindNumber(T *src)
    requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
  {
    Value val;
    val._getNumber = [=]() -> double { return (double)*src; };
    val._setNumber = [=](double value) -> void { *src = (T)value; };
    val._bind = true;
    val._type = TYPE::NUMBER;
    return val;
  }

  static Value bindBoolean(bool *src);

  static Value bindArray(Array *src);

  static Value bindObject(Object *src);
};
} // namespace firefly::core