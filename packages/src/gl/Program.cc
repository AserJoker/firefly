#include "gl/Program.hpp"
#include "core/Bitmap.hpp"
#include "gl/Constant.hpp"
#include "gl/MapAccess.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <runtime/Media.hpp>

using namespace firefly;
using namespace firefly::gl;

Program::Program() {
  _handle = glCreateProgram();
  _constantBitmap = new core::Bitmap();
  _constants = new Constant(_constantBitmap);
}

Program::~Program() { glDeleteProgram(_handle); }

void Program::attach(const core::AutoPtr<Shader> &shader) {
  glAttachShader(_handle, shader->getHandle());
}

bool Program::link() {
  glLinkProgram(_handle);
  int status;
  glGetProgramiv(_handle, GL_LINK_STATUS, &status);
  return status;
}

const std::string Program::getInfoLog() const {
  int size;
  glGetProgramiv(_handle, GL_INFO_LOG_LENGTH, &size);
  char *buf = new char[size];
  glGetProgramInfoLog(_handle, size, NULL, buf);
  std::string result = buf;
  delete[] buf;
  return result;
}

const uint32_t &Program::getHandle() const { return _handle; }

void Program::use() { glUseProgram(_handle); }

const uint32_t &Program::getUniformLocation(const std::string &name) {
  static uint32_t invalid = GL_INVALID_INDEX;
  if (!_locations.contains(name)) {
    uint32_t loc = glGetUniformLocation(_handle, name.c_str());
    if (loc == GL_INVALID_INDEX) {
      return invalid;
    }
    _locations[name] = loc;
  }
  return _locations.at(name);
}

void Program::bindUniformBlock(const std::string &name, uint32_t index) {
  if (!_uniformBlockLocations.contains(name)) {
    _uniformBlockLocations[name] =
        glGetUniformBlockIndex(_handle, name.c_str());
  }
  auto &loc = _uniformBlockLocations.at(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  if (_unformBlockBindings[loc] == index) {
    return;
  }
  _unformBlockBindings[loc] = index;
  glUniformBlockBinding(_handle, loc, index);
}

void Program::setUniform(const std::string &name, int32_t value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform1i(loc, value);
}
void Program::setUniform(const std::string &name, uint32_t value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform1ui(loc, value);
}
void Program::setUniform(const std::string &name, bool value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform1i(loc, value);
}
void Program::setUniform(const std::string &name, float value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform1f(loc, value);
}
void Program::setUniform(const std::string &name, double value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform1d(loc, value);
}
void Program::setUniform(const std::string &name, const glm::vec2 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform2fv(loc, 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::vec3 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform3fv(loc, 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::vec4 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform4fv(loc, 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat2 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix2fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat3 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix3fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat4 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix4fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::dvec2 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform2dv(loc, 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::dvec3 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform3dv(loc, 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::dvec4 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform4dv(loc, 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::ivec2 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform2iv(loc, 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::ivec3 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform3iv(loc, 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::ivec4 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform4iv(loc, 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::uvec2 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform2uiv(loc, 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::uvec3 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform3uiv(loc, 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::uvec4 &value) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniform4uiv(loc, 1, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat2x3 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix2x3fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat3x2 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix3x2fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat3x4 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix3x4fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(const std::string &name, const glm::mat4x3 &value,
                         const bool &transpose) {
  auto loc = getUniformLocation(name);
  if (loc == GL_INVALID_INDEX) {
    return;
  }
  glUniformMatrix4x3fv(loc, 1, transpose, glm::value_ptr(value));
}
void Program::setUniform(core::AutoPtr<Constant> &constants) {
  constants->sync(_constants);
  for (auto &[name, _] : _constantBitmap->getData()) {
    auto &field = _constants->getField(name);
    auto type = _constants->getFieldType(name);
    switch (type) {
    case CONSTANT_TYPE::BOOL:
      this->setUniform(name, std::any_cast<bool>(field));
      break;
    case CONSTANT_TYPE::INT:
      this->setUniform(name, std::any_cast<int>(field));
      break;
    case CONSTANT_TYPE::IVEC2:
      this->setUniform(name, std::any_cast<glm::ivec2>(field));
      break;
    case CONSTANT_TYPE::IVEC3:
      this->setUniform(name, std::any_cast<glm::ivec3>(field));
      break;
    case CONSTANT_TYPE::IVEC4:
      this->setUniform(name, std::any_cast<glm::ivec4>(field));
      break;
    case CONSTANT_TYPE::UINT:
      this->setUniform(name, std::any_cast<uint32_t>(field));
      break;
    case CONSTANT_TYPE::UIVEC2:
      this->setUniform(name, std::any_cast<glm::uvec2>(field));
      break;
    case CONSTANT_TYPE::UIVEC3:
      this->setUniform(name, std::any_cast<glm::uvec3>(field));
      break;
    case CONSTANT_TYPE::UIVEC4:
      this->setUniform(name, std::any_cast<glm::uvec4>(field));
      break;
    case CONSTANT_TYPE::FLOAT:
      this->setUniform(name, std::any_cast<float>(field));
      break;
    case CONSTANT_TYPE::VEC2:
      this->setUniform(name, std::any_cast<glm::vec2>(field));
      break;
    case CONSTANT_TYPE::VEC3:
      this->setUniform(name, std::any_cast<glm::vec3>(field));
      break;
    case CONSTANT_TYPE::VEC4:
      this->setUniform(name, std::any_cast<glm::vec4>(field));
      break;
    case CONSTANT_TYPE::DOUBLE:
      this->setUniform(name, std::any_cast<double>(field));
      break;
    case CONSTANT_TYPE::DVEC2:
      this->setUniform(name, std::any_cast<glm::dvec2>(field));
      break;
    case CONSTANT_TYPE::DVEC3:
      this->setUniform(name, std::any_cast<glm::dvec3>(field));
      break;
    case CONSTANT_TYPE::DVEC4:
      this->setUniform(name, std::any_cast<glm::dvec4>(field));
      break;
    case CONSTANT_TYPE::MATRIX2:
      this->setUniform(name, std::any_cast<glm::mat2>(field));
      break;
    case CONSTANT_TYPE::MATRIX2x3:
      this->setUniform(name, std::any_cast<glm::mat2x3>(field));
      break;
    case CONSTANT_TYPE::MATRIX3x2:
      this->setUniform(name, std::any_cast<glm::mat3x2>(field));
      break;
    case CONSTANT_TYPE::MATRIX3:
      this->setUniform(name, std::any_cast<glm::mat3>(field));
      break;
    case CONSTANT_TYPE::MATRIX3x4:
      this->setUniform(name, std::any_cast<glm::mat3x4>(field));
      break;
    case CONSTANT_TYPE::MATRIX4x3:
      this->setUniform(name, std::any_cast<glm::mat4x3>(field));
      break;
    case CONSTANT_TYPE::MATRIX4:
      this->setUniform(name, std::any_cast<glm::mat4>(field));
      break;
    case CONSTANT_TYPE::BLOCK: {
      auto block = std::any_cast<core::AutoPtr<ConstantBlock>>(field);
      this->bindUniformBlock(name, block->getIndex());
    } break;
    }
  }
  _constantBitmap->clear();
}