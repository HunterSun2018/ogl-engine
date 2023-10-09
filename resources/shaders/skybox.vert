#version 460 core

layout(location = 0) in vec3 in_position;
layout(location = 0) out vec3 out_tex_coords;

layout(std140, binding = 0) //
uniform State {
    mat4 projection;
    mat4 view;
} state;

void main() {
    gl_Position = state.projection * state.view * vec4(in_position, 1.0);
    out_tex_coords = in_position;
}