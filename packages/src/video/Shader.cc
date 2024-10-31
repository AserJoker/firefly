#include "video/Shader.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "gl/Program.hpp"
#include "gl/Shader.hpp"
#include "gl/ShaderType.hpp"
#include "runtime/Media.hpp"
#include "video/CompileException.hpp"
#include "video/LinkException.hpp"
#include <filesystem>

using namespace firefly;
using namespace firefly::video;

const core::Map<core::String_t, Shader::ShaderSource>
Shader::load(const core::String_t &path) {
  core::Map<core::String_t, ShaderSource> sources;
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
        programSources[dtype] = core::String_t(
            (core::CString_t)buffer->getData(), buffer->getSize());
      }
    }
  }
  return sources;
}

Shader::Shader(const core::String_t &path) : Shader(load(path)) {}

Shader::Shader(const core::Map<core::String_t, ShaderSource> &sources)
    : _sources(sources) {
  for (auto &[name, sources] : sources) {
    core::AutoPtr<gl::Program> program = new gl::Program();
    for (auto &[type, source] : sources) {
      core::AutoPtr<gl::Shader> shader = new gl::Shader(type);
      shader->setShaderSource(source);
      if (!shader->compile()) {
        throw CompileException(shader->getInfoLog());
      }
      program->attach(shader);
    }
    if (!program->link()) {
      throw LinkException(fmt::format("Failed to link '{}':\n\t{}", name,
                                      program->getInfoLog()));
    }
    _programs[name] = program;
  }
}

void Shader::setSource(const core::String_t &name, const gl::SHADER_TYPE &type,
                       const core::String_t &source) {}

const core::Map<core::String_t, Shader::ShaderSource> &
Shader::getSources() const {
  return _sources;
}
const core::Map<core::String_t, core::AutoPtr<gl::Program>> &
Shader::getPrograms() const {
  return _programs;
}
core::Map<core::String_t, core::AutoPtr<gl::Program>> &Shader::getPrograms() {
  return _programs;
}

core::AutoPtr<gl::Program> Shader::getProgram(const core::String_t &name) {
  if (_programs.contains(name)) {
    return _programs.at(name);
  }
  return nullptr;
}