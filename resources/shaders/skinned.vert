//
// Phong Lighting Model for vertex shader
//
#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_BONES 100

layout(std140, binding = 0) 
uniform matrix_state {
	mat4 vmat;
	mat4 projmat;
	mat4 mvmat;
	mat4 mvpmat;
	mat4 bones[MAX_BONES];
} matrix;

layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in ivec4 in_bones;
layout(location = 4) in vec4 in_weights;

out vec3 out_vpos;
out vec3 out_norm;
out vec2 out_texcoord;

void main()
{
	gl_Position = matrix.mvpmat * vec4(attr_vertex, 1.0f);

	vpos = (matrix.mvmat * vec4(attr_vertex, 1.0f)).xyz;

	norm = mat3(matrix.mvmat) * attr_normal;
	
	ldir = matrix.light_pos - vpos;

	texcoord = attr_texcoord;
}

