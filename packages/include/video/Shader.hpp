#pragma once
#include "core/AutoPtr.hpp"
#include "core/Cache.hpp"
#include "core/Object.hpp"
#include "gl/Program.hpp"
#include "gl/ShaderType.hpp"
#include <unordered_map>
namespace firefly::video {
class Shader : public core::Object, public core::Cache<Shader> {
public:
  using ShaderSource = std::unordered_map<gl::SHADER_TYPE, std::string>;

private:
  std::unordered_map<std::string, ShaderSource> _sources;
  std::unordered_map<std::string, core::AutoPtr<gl::Program>> _programs;

private:
  static const std::unordered_map<std::string, ShaderSource>
  load(const std::string &path);

public:
  Shader(const std::string &path);

  Shader(const std::unordered_map<std::string, ShaderSource> &sources);

  void setSource(const std::string &name, const gl::SHADER_TYPE &type,
                 const std::string &source);

  const std::unordered_map<std::string, ShaderSource> &getSources() const;

  const std::unordered_map<std::string, core::AutoPtr<gl::Program>> &
  getPrograms() const;

  std::unordered_map<std::string, core::AutoPtr<gl::Program>> &getPrograms();

  core::AutoPtr<gl::Program> getProgram(const std::string &name);
};
}; // namespace firefly::video