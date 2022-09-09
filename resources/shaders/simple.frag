#version 450
//
//  Input layout
//
layout(location=0)  in vec3 in_color;

//
//  Output layout
//
layout(location = 0) out vec4 out_color;

//
//  Uniform layout
//

void main()
{
    out_color = vec4(in_color, 1.0f);
}