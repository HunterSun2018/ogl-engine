#version 450 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in ivec4 BoneIDs;
layout(location = 4) in vec4 Weights;

layout(location = 0)  out vec2 TexCoords;

const int MAX_BONES = 100;

layout(std140,binding = 0) uniform State {
    mat4 gBones[MAX_BONES];
    mat4 model;
    mat4 view;
    mat4 projection;
} state;


void main() {
    
    //FragNormal = Normal;
    //FragPos = vec3(model * vec4(Position, 1.0));

    mat4 BoneTransform = state.gBones[BoneIDs[0]] * Weights[0];
    BoneTransform += state.gBones[BoneIDs[1]] * Weights[1];
    BoneTransform += state.gBones[BoneIDs[2]] * Weights[2];
    BoneTransform += state.gBones[BoneIDs[3]] * Weights[3];

    vec4 PosL = BoneTransform * vec4(Position, 1.0);
	//vec4 PosL = vec4(Position, 1.0) + BoneTransform * vec4(Position, 1.0);
	//vec4 PosL = vec4(Position, 1.0);
    gl_Position = state.projection * state.view * state.model * PosL;
	//gl_Position = projection * view * model * vec4(Position, 1.0);
    TexCoords = aTexCoords;
}