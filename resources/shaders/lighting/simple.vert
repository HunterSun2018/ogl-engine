#version 400 core

layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//in vec4 vPosition;
//in vec4 vColor;
//out vec4 color;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
} 
