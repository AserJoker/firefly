#include "video/Material.hpp"
using namespace firefly;
using namespace firefly::video;
Material::Material(const std::string &type) : _type(type) {}
const std::string &Material::getType() const { return _type; }