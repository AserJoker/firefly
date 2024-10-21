#pragma once
#include "core/AutoPtr.hpp"
#include "core/Cache.hpp"
#include "core/Map.hpp"
#include "core/Object.hpp"
#include "gl/Program.hpp"
#include "gl/ShaderType.hpp"

namespace firefly::video {
class Shader : public core::Object, public core::Cache<Shader> {
public:
  using ShaderSource = core::Map<gl::SHADER_TYPE, std::string>;

private:
  core::Map<std::string, ShaderSource> _sources;
  core::Map<std::string, core::AutoPtr<gl::Program>> _programs;

private:
  static const core::Map<std::string, ShaderSource>
  load(const std::string &path);

public:
  Shader(const std::string &path);

  Shader(const core::Map<std::string, ShaderSource> &sources);

  void setSource(const std::string &name, const gl::SHADER_TYPE &type,
                 const std::string &source);

  const core::Map<std::string, ShaderSource> &getSources() const;

  const core::Map<std::string, core::AutoPtr<gl::Program>> &getPrograms() const;

  core::Map<std::string, core::AutoPtr<gl::Program>> &getPrograms();

  core::AutoPtr<gl::Program> getProgram(const std::string &name);
};
}; // namespace firefly::video