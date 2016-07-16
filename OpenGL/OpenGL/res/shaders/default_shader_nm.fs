#version 420 core
#extension GL_ARB_explicit_uniform_location : enable

struct Light
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
};

struct DirectionalLight
{
	Light light;
};
uniform DirectionalLight directionalLight;

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

struct Maps
{
	sampler2D diffuse;
	sampler2D normal;
	sampler2D specular;	
};
uniform Maps maps;

in VS_OUT
{
	vec4 position;		
	vec2 texCoords;
	mat3 TBN;
}fs_in;

// Fragment shader uniforms base: 20
layout(location = 20) uniform vec3 eyePosition;

out vec4 fragColor;

void Phong(in Light light, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)
{
	vec3 n;
	n = texture(maps.normal, fs_in.texCoords).rgb;
	n = normalize(n * 2.0f - 1.0f);
	n = normalize(fs_in.TBN * n);
	vec4 normal = vec4(n, 0.0f);
	
	vec4 lightVector, viewVector, reflectVector;
	float diff, spec;

	lightVector = normalize(vec4(light.position, 1.0f) - fs_in.position);
	diff = max(dot(normal, lightVector), 0.0f);

	viewVector    = normalize(vec4(eyePosition, 1.0f) - fs_in.position);
	reflectVector = normalize(reflect(-lightVector, normal));	
	float specularStrength 	= 2;
	spec = specularStrength * pow(max(dot(viewVector, reflectVector), 0.0f), material.shininess);

	ambient  += 	   vec4(light.ambient, 1.0f);
	diffuse  += diff * vec4(light.diffuse, 1.0f);
	specular += spec * vec4(light.specular, 1.0f);	
}

void Blinn_Phong(in Light light, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)
{
	vec3 n;
	n = texture(maps.normal, fs_in.texCoords).rgb;
	n = normalize(n * 2.0f - 1.0f);
	n = normalize(fs_in.TBN * n);
	vec4 normal = vec4(n, 0.0f);
	
	vec4 lightVector, viewVector, halfwayVector;
	float diff, spec;

	lightVector = normalize(vec4(light.position, 1.0f) - fs_in.position);
	diff = max(dot(normal, lightVector), 0.0f);

	viewVector    = normalize(vec4(eyePosition, 1.0f) - fs_in.position);
	halfwayVector = normalize(lightVector + viewVector);
	spec = pow(max(dot(normal, halfwayVector), 0.0f), material.shininess);

	ambient  += 	   vec4(light.ambient, 1.0f);
	diffuse  += diff * vec4(light.diffuse, 1.0f);
	specular += spec * vec4(light.specular, 1.0f);	
}

void main()
{	
	vec4 ambient, diffuse, specular;	
	ambient = diffuse = specular = vec4(0.0f);
	Blinn_Phong(directionalLight.light, ambient, diffuse, specular);

	// vec4 waterColor = vec4(0.19f, 0.34f, 0.75f, 1.0f);
    fragColor = (ambient + diffuse + specular) * texture(maps.diffuse, fs_in.texCoords);   
}