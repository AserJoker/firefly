#include "video/Lighting.hpp"
using namespace firefly::video;

Lighting::Lighting() : _shininess(0) {}
glm::vec3 &Lighting::getPosition() { return _position; }
const glm::vec3 &Lighting::getPositon() const { return _position; }
glm::vec3 &Lighting::getAmbient() { return _ambient; }
const glm::vec3 &Lighting::getAmbient() const { return _ambient; }
glm::vec3 &Lighting::getDiffuse() { return _diffuse; }
const glm::vec3 &Lighting::getDiffuse() const { return _diffuse; }
glm::vec3 &Lighting::getSpecular() { return _specular; }
const glm::vec3 &Lighting::getSpecular() const { return _specular; }
float &Lighting::getShininess() { return _shininess; }
const float &Lighting::getShininess() const { return _shininess; }