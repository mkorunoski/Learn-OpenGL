#version 420 core

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

struct Maps
{
	sampler2D diffuse;	
	sampler2D normal;
	sampler2D specular;
};

in vec4 f_position;
in vec4 f_normal;
in vec2 f_texCoords;
in mat4 f_inverseTranspose;

uniform vec3 eyePosition;
uniform int whichLight;
uniform bool normalFromMap;
uniform bool reflection;

uniform Material material;

#define NUM_POINT_LIGHTS 3

uniform DirectionalLight directionalLight;
uniform PointLight pointLight[NUM_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform Maps maps;

out vec4 fragColor;

float LinearizeDepth(float depth)
{
	float near = 0.1;
	float far = 100.0;
	float z = depth * 2.0 - 1.0;
	return (2.0 * near) / (far + near - z * (far - near));
}

void Phong(in Light light, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)
{
	vec4 normal = normalize(f_normal);
	if (normalFromMap)
	{
		// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
	}

	vec4 lightVector, viewVector, reflectVector;
	float diff, spec;

	lightVector = normalize(light.position - f_position);
	diff = max(dot(normal, lightVector), 0.1f);

	viewVector    = normalize(vec4(eyePosition, 1.0f) - f_position);
	reflectVector = normalize(reflect(-lightVector, normal));
	spec = pow(max(dot(viewVector, reflectVector), 0.1f), material.shininess);

	ambient  *= light.ambient;
	diffuse  *= diff * light.diffuse;
	specular *= spec * light.specular;	
}

void CalcPointLight(in PointLight pointLight, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)
{
	Phong(pointLight.light, ambient, diffuse, specular);
	float dist = length(pointLight.light.position - f_position);
	float atenuation = 1.0f / (pointLight.constant + pointLight.linear * dist + pointLight.quadratic * pow(dist, 2));
	ambient  *= atenuation;
	diffuse  *= atenuation;
	specular *= atenuation;
}

void CalcSpotLight(in SpotLight spotLight, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)
{
	vec4 lightVector = normalize(spotLight.light.position - f_position);
	float theta = dot(lightVector, normalize(-spotLight.direction));		
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
		ambient  = spotLight.light.ambient;
		diffuse  = vec4(0.0f);
		specular = vec4(0.0f);
	}
}

void main()
{	
	vec4 ambient  = material.ambient;
	vec4 diffuse  = material.diffuse;
	vec4 specular = material.specular * SPECULAR_STRENGTH * texture2D(maps.specular, f_texCoords);
	// ===========================================================
	// directional
	// ===========================================================
	if(whichLight == 1)
	{
		Phong(directionalLight.light, ambient, diffuse, specular);
	}
	// ===========================================================
	// point 
	// ===========================================================
	if(whichLight == 2)
	{	
		vec4 a, d, s;
		vec4 resa = vec4(0.0f), resd = vec4(0.0f), ress = vec4(0.0f);
		for(int i = 0; i < NUM_POINT_LIGHTS; ++i)
		{
			a = d = s = vec4(1.0f);
			CalcPointLight(pointLight[i], a, d, s);
			resa += a;
			resd += d;
			ress += s;
		}
		ambient  *= resa;
		diffuse  *= resd;
		specular *= ress;
	}
	// ===========================================================
	// spot 
	// ===========================================================
	if(whichLight == 3)
	{
		CalcSpotLight(spotLight, ambient, diffuse, specular);
	}

    fragColor = (ambient + diffuse + specular) * vec4(texture2D(maps.diffuse, f_texCoords).rgb, 0.4f);

    //float depth = LinearizeDepth(gl_FragCoord.z);
    //fragColor = vec4(vec3(depth), 1.0f);

    if(reflection)
    {
    	vec4 above = vec4(f_position.x, 0, f_position.z, 1.0f);
    	float len = length(above - f_position);
    	fragColor *= 1.0f / len;
    }
}