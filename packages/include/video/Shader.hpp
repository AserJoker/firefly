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
  using ShaderSource = core::Map<gl::SHADER_TYPE, core::String_t>;

private:
  core::Map<core::String_t, ShaderSource> _sources;
  core::Map<core::String_t, core::AutoPtr<gl::Program>> _programs;

private:
  static const core::Map<core::String_t, ShaderSource>
  load(const core::String_t &path);

public:
  Shader(const core::String_t &path);

  Shader(const core::Map<core::String_t, ShaderSource> &sources);

  void setSource(const core::String_t &name, const gl::SHADER_TYPE &type,
                 const core::String_t &source);

  const core::Map<core::String_t, ShaderSource> &getSources() const;

  const core::Map<core::String_t, core::AutoPtr<gl::Program>> &
  getPrograms() const;

  core::Map<core::String_t, core::AutoPtr<gl::Program>> &getPrograms();

  core::AutoPtr<gl::Program> getProgram(const core::String_t &name);
};
}; // namespace firefly::video