#ifndef LIGHT_H
#define LIGHT_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

template <typename T>
std::string Str(const T & t) {
	std::ostringstream os;
	os << t;
	return os.str();
}

class DirectionalLight
{
public:
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 position;

	GLuint uAmbient;
	GLuint uDiffuse;
	GLuint uSpecular;
	GLuint uPosition;

public:
	DirectionalLight(const GLuint& program,
		const glm::vec4& ambient, const glm::vec4& diffuse, const glm::vec4& specular,
		const glm::vec4& position)
	{
		this->ambient  = ambient;
		this->diffuse  = diffuse;
		this->specular = specular;
		this->position = position;

		uAmbient = glGetUniformLocation(program, "directionalLight.light.ambient");
		uDiffuse = glGetUniformLocation(program, "directionalLight.light.diffuse");
		uSpecular = glGetUniformLocation(program, "directionalLight.light.specular");
		uPosition = glGetUniformLocation(program, "directionalLight.light.position");
	}

	void Use()
	{
		glUniform4fv(uAmbient, 1, glm::value_ptr(ambient));
		glUniform4fv(uDiffuse, 1, glm::value_ptr(diffuse));
		glUniform4fv(uSpecular, 1, glm::value_ptr(specular));
		glUniform4fv(uPosition, 1, glm::value_ptr(position));
	}

	void InvertY()
	{
		this->position.y = -1 * this->position.y;
	}

	~DirectionalLight() { }
};

class PointLight
{
public:
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 position;
	float constant;
	float linear;
	float quadratic;

	GLuint uAmbient;
	GLuint uDiffuse;
	GLuint uSpecular;
	GLuint uPosition;
	GLuint uConstant;
	GLuint uLinear;
	GLuint uQuadratic;
	
public:
	PointLight(const GLuint& program, int light,
		const glm::vec4& ambient, const glm::vec4& diffuse, const glm::vec4& specular,
		const glm::vec4& position, float constant, float linear, float quadratic)
	{
		this->ambient   = ambient;
		this->diffuse   = diffuse;
		this->specular  = specular;
		this->position  = position;
		this->constant  = constant;
		this->linear    = linear;
		this->quadratic = quadratic;
		
		uAmbient   = glGetUniformLocation(program, ("pointLight[" + Str(light) + "].light.ambient").c_str());
		uDiffuse   = glGetUniformLocation(program, ("pointLight[" + Str(light) + "].light.diffuse").c_str());
		uSpecular  = glGetUniformLocation(program, ("pointLight[" + Str(light) + "].light.specular").c_str());
		uPosition  = glGetUniformLocation(program, ("pointLight[" + Str(light) + "].light.position").c_str());
		uConstant  = glGetUniformLocation(program, ("pointLight[" + Str(light) + "].constant").c_str());
		uLinear    = glGetUniformLocation(program, ("pointLight[" + Str(light) + "].linear").c_str());
		uQuadratic = glGetUniformLocation(program, ("pointLight[" + Str(light) + "].quadratic").c_str());
	}

	void Use()
	{
		glUniform4fv(uAmbient, 1, glm::value_ptr(ambient));
		glUniform4fv(uDiffuse, 1, glm::value_ptr(diffuse));
		glUniform4fv(uSpecular, 1, glm::value_ptr(specular));
		glUniform4fv(uPosition, 1, glm::value_ptr(position));
		glUniform1f(uConstant, constant);
		glUniform1f(uLinear, linear);
		glUniform1f(uQuadratic, quadratic);
	}

	void InvertY()
	{
		this->position.y = -1 * this->position.y;
	}

	~PointLight() { }
};

class SpotLight
{
public:
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	glm::vec4 position;
	glm::vec4 direction;
	float cutOff;
	float outerCutOff;

	GLuint uAmbient;
	GLuint uDiffuse;
	GLuint uSpecular;
	GLuint uPosition;
	GLuint uDirection;
	GLuint uCutOff;
	GLuint uOuterCutOff;

public:
	SpotLight(const GLuint& program,
		const glm::vec4& ambient, const glm::vec4& diffuse, const glm::vec4& specular,
		const glm::vec4& position, const glm::vec4& direction, float cutOff, float outerCutOff)
	{
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->position = position;
		this->direction = direction;
		this->cutOff = cutOff;
		this->outerCutOff = outerCutOff;

		uAmbient	 = glGetUniformLocation(program, "spotLight.light.ambient");
		uDiffuse	 = glGetUniformLocation(program, "spotLight.light.diffuse");
		uSpecular	 = glGetUniformLocation(program, "spotLight.light.specular");
		uPosition	 = glGetUniformLocation(program, "spotLight.light.position");
		uDirection	 = glGetUniformLocation(program, "spotLight.direction"); 
		uCutOff		 = glGetUniformLocation(program, "spotLight.cutOff"); 
		uOuterCutOff = glGetUniformLocation(program, "spotLight.outerCutOff");;
	}
	
	void Use()
	{
		glUniform4fv(uAmbient, 1, glm::value_ptr(ambient));
		glUniform4fv(uDiffuse, 1, glm::value_ptr(diffuse));
		glUniform4fv(uSpecular, 1, glm::value_ptr(specular));
		glUniform4fv(uPosition, 1, glm::value_ptr(position));
		glUniform4fv(uDirection, 1, glm::value_ptr(direction));
		glUniform1f(uCutOff, glm::cos(glm::radians(cutOff)));
		glUniform1f(uOuterCutOff, glm::cos(glm::radians(outerCutOff)));
	}

	~SpotLight() { }
};

#endif