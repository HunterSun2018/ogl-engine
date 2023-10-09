#version 450 core

layout(location=0) in vec2 TexCoords;

layout(location=0) out vec4 FragColor;


layout(binding = 0) uniform sampler2D texture_diffuse1;

void main()
{   
    //vec3 norm = normalize(FragNormal);
    //vec3 lightDir = normalize(lightPos - FragPos); 

    //float diff = max(dot(norm, lightDir), 0.0);
    //vec3 diffuse = diff * lightColor;
    
    //vec3 ambient = ambientStrength * ambientColor;
    
    //vec3 result = (ambient + diffuse) * vec3(0.2f, 0.2f, 1.0f);
    //FragColor = vec4(texture(texture_diffuse1, TexCoords));
	FragColor = vec4(1.0, 1.0, 1.0, 1.0f);
}