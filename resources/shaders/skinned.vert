//
// Phong Lighting Model for vertex shader
//
#version 450

//#extension GL_ARB_separate_shader_objects : enable
//#extension GL_EXT_scalar_block_layout : enable

//
// Input layout
//
layout(location = 0) in vec3 	in_vertex;
layout(location = 1) in vec3 	in_normal;
layout(location = 2) in vec2 	in_texcoord;
layout(location = 3) in ivec4 	in_bone_ids;
layout(location = 4) in vec4 	in_weights;
//
//	Output layout
//
layout(location = 0) out vec3 out_pos;
layout(location = 1) out vec3 out_norm;
layout(location = 2) out vec2 out_texcoord;
//
//	Uniform
//
#define MAX_BONES 100
layout(std140, binding = 0) 
uniform State {
	mat4 bones[MAX_BONES];
	mat4 vm;	// view * model
	mat4 pvm;	// projection * view * model	
} state;

void main()
{
	//
	//	Combine bone matrices
	//	
	mat4 bone_transform = state.bones[in_bone_ids[0]] * in_weights[0];
    bone_transform += state.bones[in_bone_ids[1]] * in_weights[1];
    bone_transform += state.bones[in_bone_ids[2]] * in_weights[2];
    bone_transform += state.bones[in_bone_ids[3]] * in_weights[3];

	//
	//	output
	//
	gl_Position = state.pvm * bone_transform * vec4(in_vertex, 1.0f);
	
	out_pos = (state.vm * bone_transform * vec4(in_vertex, 1.0f)).xyz;

	out_norm = (state.vm * bone_transform * vec4(in_normal, 0.f)).xyz;
		
	out_texcoord = in_texcoord;	
}

