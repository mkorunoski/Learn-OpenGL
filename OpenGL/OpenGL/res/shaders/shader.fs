#version 330 core

const float SPECULAR_STRENGTH = 2;

struct Material
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

struct Light
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
};

struct DirectionalLight
{
	Light light;
};

struct PointLight
{
	Light light;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight
{
	Light light;

	vec4 direction;
	float cutOff;
	float outerCutOff;
};

in vec4 f_position;
in vec4 f_normal;
in vec2 f_texCoords;

uniform vec3 eyePos;
uniform int numLights;

uniform sampler2D texture;
uniform sampler2D specularLM;

uniform Material material;
uniform DirectionalLight directionalLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

out vec4 fragColor;

void CalcLight(in Light light, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)
{
	vec4 normal = normalize(f_normal);
	vec4 lightVector, viewVector, reflectVector;
	float diff, spec;

	lightVector = normalize(light.position - f_position);
	diff = max(dot(normal, lightVector), 0.1f);

	viewVector    = normalize(vec4(eyePos, 1.0f) - f_position);
	reflectVector = normalize(reflect(-lightVector, normal));
	spec = pow(max(dot(viewVector, reflectVector), 0.1f), material.shininess);

	ambient  *= light.ambient;
	diffuse  *= diff * light.diffuse;
	specular *= spec * light.specular;	
}

void main()
{	
	vec4 ambient  = material.ambient;
	vec4 diffuse  = material.diffuse;
	vec4 specular = material.specular * SPECULAR_STRENGTH * texture2D(specularLM, f_texCoords);
	// ===========
	// directional 
	// ===========
	if(numLights == 1)
	{
		CalcLight(directionalLight.light, ambient, diffuse, specular);
	}
	// ===========
	// point 
	// ===========
	if(numLights == 2)
	{
		CalcLight(pointLight.light, ambient, diffuse, specular);
		float dist = length(pointLight.light.position - f_position);
		float atenuation = 1.0f / (pointLight.constant + pointLight.linear * dist + pointLight.quadratic * pow(dist, 2));
		ambient  *= atenuation;
		diffuse  *= atenuation;
		specular *= atenuation;
	}
	// ===========
	// spot 
	// ===========
	if(numLights == 3)
	{
		vec4 lightVector = normalize(spotLight.light.position - f_position);
		float theta = dot(lightVector, normalize(-spotLight.direction));		
		if(theta > spotLight.cutOff)
		{
			CalcLight(spotLight.light, ambient, diffuse, specular);
			float epsilon = spotLight.cutOff - spotLight.outerCutOff;
			float intensity = clamp((spotLight.cutOff - theta) / epsilon, 0.0f, 1.0f);
			diffuse  *= intensity;
			specular *= intensity;
		}
		else
		{
			ambient  *= spotLight.light.ambient;
			diffuse  = vec4(0.0f);
			specular = vec4(0.0f);
		}
	}

    fragColor =  (ambient + diffuse + specular) * texture2D(texture, f_texCoords);
}