#version 450 core

layout(location = 0) in vec3 in_tex_coords;
layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform samplerCube skybox;

void main() {
    out_color = texture(skybox, in_tex_coords);
}