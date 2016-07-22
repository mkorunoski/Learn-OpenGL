#version 420 core
#extension GL_ARB_explicit_uniform_location : enable

#define NUM_POINT_LIGHTS 3
#define SPECULAR_STRENGTH 4

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
	sampler2D shadow;
};
uniform Maps maps;

in VS_OUT
{
	vec4 position;
	vec4 positionLightSpace;
	vec4 normal;
} fs_in;

// Fragment shader uniforms base: 20
layout(location = 20) uniform vec3 			eyePosition;

// Texture samplers base: 30
layout(location = 30) uniform samplerCube 	skyboxTex;

uniform bool reflection;

out vec4 fragColor;

void Blinn_Phong(in Light light, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)
{
	vec4 normal = normalize(fs_in.normal);
	
	vec4 lightVector, viewVector, halfwayVector;
	float diff, spec;

	lightVector = normalize(vec4(light.position, 1.0f) - fs_in.position);
	diff = max(dot(normal, lightVector), 0.0f);

	viewVector    = normalize(vec4(eyePosition, 1.0f) - fs_in.position);
	halfwayVector = normalize(lightVector + viewVector);
	spec = pow(max(dot(normal, halfwayVector), 0.0f), material.shininess);

	ambient  += 	   vec4(light.ambient, 1.0f);
	diffuse  += diff * vec4(light.diffuse, 1.0f);
	specular += SPECULAR_STRENGTH * spec * vec4(light.specular, 1.0f);	
}

float ShadowCalculation(vec4 fragPosLightSpace);

void CalcPointLight(in PointLight pointLight, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular);
void CalcSpotLight(in SpotLight spotLight, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular);

void main()
{
	vec4 ambient, diffuse, specular;	
	ambient = diffuse = specular = vec4(0.0f);

	vec4 tmpAmbient, tmpDiffuse, tmpSpecular;

	tmpAmbient = tmpDiffuse = tmpSpecular = vec4(0.0f);
	Blinn_Phong(directionalLight.light, tmpAmbient, tmpDiffuse, tmpSpecular);
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

	vec4 incident = normalize(fs_in.position - vec4(eyePosition, 1.0f));
	vec4 refl = reflect(incident, normalize(fs_in.normal));
	float ratio = 1.00f / 1.52f;    
    vec4 refr = refract(incident, normalize(fs_in.normal), ratio);
    float a = 0.9f;
    float ia = 1.0f - a;
	if(reflection) 
		fragColor = (ambient + diffuse + specular);
	else
	{
		float shadow = ShadowCalculation(fs_in.positionLightSpace);
		fragColor = (ambient + (1.0f - shadow) * (diffuse + specular));
	}
    fragColor *= (a * texture(skyboxTex, refl.xyz) + ia * texture(skyboxTex, refr.xyz));
}





float ShadowCalculation(vec4 fragPosLightSpace)
{
	// perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(maps.shadow, projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check whether current frag pos is in shadow
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    return shadow;	
}

void CalcPointLight(in PointLight pointLight, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)
{
	Blinn_Phong(pointLight.light, ambient, diffuse, specular);
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
		Blinn_Phong(spotLight.light, ambient, diffuse, specular);
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