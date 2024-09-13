#include "video/Shader.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "exception/Exception.hpp"
#include "gl/Program.hpp"
#include "gl/Shader.hpp"
#include "gl/ShaderType.hpp"
#include "runtime/Media.hpp"
#include <filesystem>
#include <string>
#include <unordered_map>
using namespace firefly;
using namespace firefly::video;

const std::unordered_map<std::string, Shader::ShaderSource>
Shader::load(const std::string &path) {
  std::unordered_map<std::string, ShaderSource> sources;
  auto media = core::Singleton<runtime::Media>::instance();
  auto programs = media->scan(path);
  for (auto &program : programs) {
    auto pos = program.find_last_of("::");
    auto name = program.substr(pos + 1);
    auto items = media->scan(program);
    for (auto item : items) {
      auto files = media->resolve(item);
      std::filesystem::path filepath = files[files.size() - 1];
      auto filename = filepath.filename().string();
      if (!std::filesystem::is_directory(filepath) &&
          filepath.extension() == ".glsl") {
        auto &programSources = sources[name];
        auto stype = filename.substr(0, filename.length() - 5);
        gl::SHADER_TYPE dtype;
        if (stype == "vertex") {
          dtype = gl::SHADER_TYPE::VERTEX_SHADER;
        } else if (stype == "fragment") {
          dtype = gl::SHADER_TYPE::FRAGMENT_SHADER;
        } else if (stype == "tess_control") {
          dtype = gl::SHADER_TYPE::TESS_CONTROL_SHADER;
        } else if (stype == "tess_evaluation") {
          dtype = gl::SHADER_TYPE::TESS_EVALUATION_SHADER;
        } else if (stype == "geometry") {
          dtype = gl::SHADER_TYPE::GEOMETRY_SHADER;
        } else {
          continue;
        }
        auto buffer = media->load(item)->read();
        programSources[dtype] =
            std::string((const char *)buffer->getData(), buffer->getSize());
      }
    }
  }
  return sources;
}

Shader::Shader(const std::string &path) : Shader(load(path)) {}

Shader::Shader(const std::unordered_map<std::string, ShaderSource> &sources)
    : _sources(sources) {
  for (auto &[name, sources] : sources) {
    core::AutoPtr<gl::Program> program = new gl::Program();
    for (auto &[type, source] : sources) {
      core::AutoPtr<gl::Shader> shader = new gl::Shader(type);
      shader->setShaderSource(source);
      if (!shader->compile()) {
        throw exception::RuntimeException<"Shader Compile">(
            shader->getInfoLog());
      }
      program->attach(shader);
    }
    if (!program->link()) {
      throw exception::RuntimeException<"Program Link">(fmt::format(
          "Failed to link '{}':\n\t{}", name, program->getInfoLog()));
    }
    _programs[name] = program;
  }
}

void Shader::setSource(const std::string &name, const gl::SHADER_TYPE &type,
                       const std::string &source) {}

const std::string Shader::preCompile(const std::string &source) {
  std::string result = source;
  return result;
}

const std::unordered_map<std::string, Shader::ShaderSource> &
Shader::getSources() const {
  return _sources;
}
const std::unordered_map<std::string, core::AutoPtr<gl::Program>> &
Shader::getPrograms() const {
  return _programs;
}
std::unordered_map<std::string, core::AutoPtr<gl::Program>> &
Shader::getPrograms() {
  return _programs;
}

core::AutoPtr<gl::Program> Shader::active(const std::string &name) {
  if (_programs.contains(name)) {
    auto program = _programs.at(name);
    program->use();
    return program;
  }
  return nullptr;
}