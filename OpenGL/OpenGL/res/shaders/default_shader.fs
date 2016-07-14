#version 420 core
#extension GL_ARB_explicit_uniform_location : enable

#define NUM_POINT_LIGHTS 3

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

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

struct PointLight
{
	Light light;

	float constant;
	float linear;
	float quadratic;
};
uniform PointLight pointLight[NUM_POINT_LIGHTS];

struct SpotLight
{
	Light light;

	vec3 direction;
	float cutOff;
	float outerCutOff;
};
uniform SpotLight spotLight;

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
	vec4 normal;
	vec2 texCoords;
} fs_in;

// Fragment shader uniforms base: 20
layout(location = 20) uniform vec3 eyePosition;

out vec4 fragColor;

void Phong(in Light light, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)
{
	vec4 normal = normalize(fs_in.normal);
	
	vec4 lightVector, viewVector, reflectVector;
	float diff, spec;

	lightVector = normalize(vec4(light.position, 1.0f) - fs_in.position);
	diff = max(dot(normal, lightVector), 0.0f);

	viewVector    = normalize(vec4(eyePosition, 1.0f) - fs_in.position);
	reflectVector = normalize(reflect(-lightVector, normal));
	spec = pow(max(dot(viewVector, reflectVector), 0.0f), material.shininess);

	ambient  += 	   vec4(light.ambient, 1.0f);
	diffuse  += diff * vec4(light.diffuse, 1.0f);
	specular += spec * vec4(light.specular, 1.0f);	
}

void CalcPointLight(in PointLight pointLight, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular);
void CalcSpotLight(in SpotLight spotLight, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular);

void main()
{	
	vec4 ambient, diffuse, specular;	
	ambient = diffuse = specular = vec4(0.0f);

	vec4 tmpAmbient, tmpDiffuse, tmpSpecular;

	tmpAmbient = tmpDiffuse = tmpSpecular = vec4(0.0f);
	Phong(directionalLight.light, tmpAmbient, tmpDiffuse, tmpSpecular);
	ambient += tmpAmbient;
	diffuse += tmpDiffuse;
	specular += tmpSpecular;		
		
	for(int i = 0; i < NUM_POINT_LIGHTS; ++i)
	{	
		tmpAmbient = tmpDiffuse = tmpSpecular = vec4(0.0f);
		CalcPointLight(pointLight[i], tmpAmbient, tmpDiffuse, tmpSpecular);
		ambient += tmpAmbient;
		diffuse += tmpDiffuse;
		specular += tmpSpecular;
	}	

	// tmpAmbient = tmpDiffuse = tmpSpecular = vec4(0.0f);
	// CalcSpotLight(spotLight, tmpAmbient, tmpDiffuse, tmpSpecular);
	// ambient += tmpAmbient;
	// diffuse += tmpDiffuse;
	// specular += tmpSpecular;

    fragColor = (ambient + diffuse + specular) * vec4(texture(maps.diffuse, fs_in.texCoords).rgb, 0.3f);
}





void CalcPointLight(in PointLight pointLight, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)
{
	Phong(pointLight.light, ambient, diffuse, specular);
	float dist = length(vec4(pointLight.light.position, 1.0f) - fs_in.position);
	float atenuation = 1.0f / (pointLight.constant + pointLight.linear * dist + pointLight.quadratic * pow(dist, 2));
	ambient  *= atenuation;
	diffuse  *= atenuation;
	specular *= atenuation;
}

void CalcSpotLight(in SpotLight spotLight, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)
{
	vec4 lightVector = normalize(vec4(spotLight.light.position, 1.0f) - fs_in.position);
	float theta = dot(lightVector, normalize(-vec4(spotLight.direction, 0.0f)));		
	if(theta > spotLight.cutOff)
	{
		Phong(spotLight.light, ambient, diffuse, specular);
		float epsilon = spotLight.cutOff - spotLight.outerCutOff;
		float intensity = clamp((spotLight.cutOff - theta) / epsilon, 0.0f, 1.0f);
		diffuse  *= intensity;
		specular *= intensity;
	}
	else
	{
		ambient  = vec4(spotLight.light.ambient, 1.0f);
		diffuse  = vec4(0.0f);
		specular = vec4(0.0f);
	}
}