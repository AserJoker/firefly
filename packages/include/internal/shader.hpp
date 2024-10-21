#pragma once
#include "core/Value.hpp"
#include "gl/ShaderType.hpp"
#include <initializer_list>
#include <utility>
namespace firefly::internal {
namespace {
constexpr static const char *vertex =
    "#version 330 core \n\r"
    "layout(location = 0) in vec3 position;\n\r"
    "layout(location = 3) in vec2 coord;\n\r"
    "out vec2 vertexTexcoord;\n\r"
    "uniform mat4 projection;\n\r"
    "uniform mat4 view;\n\r"
    "uniform mat4 model;\n\r"
    "uniform mat4 diffuse_texture_coord_matrix;\n\r"
    "void main() {\n\r"
    "gl_Position = projection * view * model * vec4(position, 1.0);\n\r"
    "vertexTexcoord = (diffuse_texture_coord_matrix * "
    "vec4(coord,0.0,1.0)).xy;\n\r"
    "}\n\r";
constexpr static const char *fragment =
    "#version 330 core\n\r"
    "in vec2 vertexTexcoord;\n\r"
    "uniform sampler2D diffuse_texture;\n\r"
    "uniform float diffuse_texture_blend;\n\r"
    "void main() {\n\r"
    "    vec4 col = texture(diffuse_texture, vertexTexcoord).rgba;\n\r"
    "    gl_FragColor = col * diffuse_texture_blend;\n\r"
    "}\n\r";
} // namespace
constexpr std::initializer_list<std::pair<gl::SHADER_TYPE, core::CString_t>>
    basic = {{gl::SHADER_TYPE::VERTEX_SHADER, vertex},
             {gl::SHADER_TYPE::FRAGMENT_SHADER, fragment}};
} // namespace firefly::internal