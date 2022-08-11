//
// Phong Lighting Model for fragment shader
//
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable


struct DirectionLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout(binding = 1) uniform Material {
    float shininess;
} material;

layout(std430, binding = 2) uniform LightSource
{ 
    DirectionLight  dirLight;
    PointLight      pointLight[4];   // The maximum limit is 4
} lightSource;

//layout(binding = 0) uniform sampler2D tex;
layout(binding = 0) uniform sampler2D tex_diffuse;
layout(binding = 1) uniform sampler2D tex_specular;

layout(location = 3) in vec3 vpos;
layout(location = 4) in vec3 normal;
layout(location = 5) in vec3 ldir;
layout(location = 6) in vec2 texcoord;

layout(location = 0) out vec4 color;

vec3 CalcDirLight(DirectionLight light, vec3 normal, vec3 viewDir);

void main()
{
	vec3 norm = normalize(normal);    
    vec3 vdir = -normalize(vpos);

    vec3 result = CalcDirLight(lightSource.dirLight, norm, vdir);

    color = vec4(result, 1.0f);
    
    //color == vec4(1.0, 0.0, 0.0, 1.0);		
}

// Calculates the color when using a directional light.
vec3 CalcDirLight(DirectionLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    // Combine results
    vec3 ambient = light.ambient * vec3(texture(tex_diffuse, texcoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(tex_diffuse, texcoord));
    vec3 specular = light.specular * spec * vec3(texture(tex_specular, texcoord));
    //return light.specular;
    return (ambient + diffuse + specular);
}

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // Combine results
    vec3 ambient = light.ambient * vec3(texture(tex_diffuse, texcoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(tex_diffuse, texcoord));
    vec3 specular = light.specular * spec * vec3(texture(tex_specular, texcoord));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}