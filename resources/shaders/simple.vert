#version 450
//
//  Input layout
//
layout(location = 0) in vec3 	in_vertex;
layout(location = 1) in vec3 	in_color;
//
//  Output layout
//
layout(location = 0) out vec3 out_color;
//
//  Unioform layout
//
layout(std140, binding = 0) 
uniform Matrix {
	mat4 vm;	// view * model
	mat4 pvm;	// project * view * model
} matrix;


void main()
{
    gl_Position = matrix.pvm * vec4(in_vertex, 1.0);
    
    out_color = in_color;
}