#pragma once
#include "core/Value.hpp"
#include "gl/ShaderType.hpp"
#include <initializer_list>
#include <utility>
namespace firefly::internal {
namespace {
constexpr static const char *vertex =
    "#version 330 core\n\r"
    "layout(location = 0) in vec3 position;\n\r"
    "layout(location = 1) in vec2 coord;\n\r"
    "uniform mat4 projection;\n\r"
    "uniform mat4 model;\n\r"
    "uniform mat4 view;\n\r"
    "out vec2 vertexTextureCoord;\n\r"
    "void main() {\n\r"
    "    gl_Position = projection * view * model * vec4(position, 1.0);\n\r"
    "    vertexTextureCoord = coord;\n\r"
    "}\n\r";
constexpr static const char *fragment =
    "#version 330 core"
    "in vec2 vertexTextureCoord;\n\r"
    "uniform sampler2D diffuse_texture;\n\r"
    "uniform int diffuse_enable;\n\r"
    "uniform mat4 diffuse_matrix;\n\r"
    "\n\r"
    "void main() {\n\r"
    "    if(diffuse_enable == 1) {\n\r"
    "        gl_FragColor = texture(diffuse_texture, (diffuse_matrix * "
    "vec4(vertexTextureCoord, 0.0, 1.0)).xy);\n\r"
    "    } else {\n\r"
    "        gl_FragColor = vec4(0, 0, 0, 1);\n\r"
    "    }\n\r"
    "}\n\r";
} // namespace
constexpr std::initializer_list<std::pair<gl::SHADER_TYPE, core::CString_t>>
    basic = {{gl::SHADER_TYPE::VERTEX_SHADER, vertex},
             {gl::SHADER_TYPE::FRAGMENT_SHADER, fragment}};
} // namespace firefly::internal