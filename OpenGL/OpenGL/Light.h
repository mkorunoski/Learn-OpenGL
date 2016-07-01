#ifndef LIGHT_H
#define LIGHT_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class DirectionalLight
{
public:
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	glm::vec4 position;

	DirectionalLight(const glm::vec4& ambient, const glm::vec4& diffuse, const glm::vec4& specular,
		const glm::vec4& position)
	{
		this->ambient  = ambient;
		this->diffuse  = diffuse;
		this->specular = specular;

		this->position = position;
	}

	void Use(const GLuint& program)
	{
		glUniform4fv(glGetUniformLocation(program, "directionalLight.light.ambient"), 1, glm::value_ptr(ambient));
		glUniform4fv(glGetUniformLocation(program, "directionalLight.light.diffuse"), 1, glm::value_ptr(diffuse));
		glUniform4fv(glGetUniformLocation(program, "directionalLight.light.specular"), 1, glm::value_ptr(specular));
		glUniform4fv(glGetUniformLocation(program, "directionalLight.light.position"), 1, glm::value_ptr(position));
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

	PointLight(const glm::vec4& ambient, const glm::vec4& diffuse, const glm::vec4& specular,
		const glm::vec4& position, float constant, float linear, float quadratic)
	{
		this->ambient   = ambient;
		this->diffuse   = diffuse;
		this->specular  = specular;

		this->position  = position;
		this->constant  = constant;
		this->linear    = linear;
		this->quadratic = quadratic;
	}

	void Use(const GLuint& program)
	{
		glUniform4fv(glGetUniformLocation(program, "pointLight.light.ambient"), 1, glm::value_ptr(ambient));
		glUniform4fv(glGetUniformLocation(program, "pointLight.light.diffuse"), 1, glm::value_ptr(diffuse));
		glUniform4fv(glGetUniformLocation(program, "pointLight.light.specular"), 1, glm::value_ptr(specular));
		glUniform4fv(glGetUniformLocation(program, "pointLight.light.position"), 1, glm::value_ptr(position));

		glUniform1f(glGetUniformLocation(program, "pointLight.constant"), constant);
		glUniform1f(glGetUniformLocation(program, "pointLight.linear"), linear);
		glUniform1f(glGetUniformLocation(program, "pointLight.quadratic"), quadratic);
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

	SpotLight(const glm::vec4& ambient, const glm::vec4& diffuse, const glm::vec4& specular,
		const glm::vec4& position, const glm::vec4& direction, float cutOff, float outerCutOff)
	{
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;

		this->position = position;
		this->direction = direction;
		this->cutOff = cutOff;
		this->outerCutOff = outerCutOff;
	}

	void Use(const GLuint& program)
	{
		glUniform4fv(glGetUniformLocation(program, "spotLight.light.ambient"), 1, glm::value_ptr(ambient));
		glUniform4fv(glGetUniformLocation(program, "spotLight.light.diffuse"), 1, glm::value_ptr(diffuse));
		glUniform4fv(glGetUniformLocation(program, "spotLight.light.specular"), 1, glm::value_ptr(specular));
		glUniform4fv(glGetUniformLocation(program, "spotLight.light.position"), 1, glm::value_ptr(position));

		glUniform4fv(glGetUniformLocation(program, "spotLight.direction"), 1, glm::value_ptr(direction));
		glUniform1f(glGetUniformLocation(program, "spotLight.cutOff"), glm::cos(glm::radians(cutOff)));
		glUniform1f(glGetUniformLocation(program, "spotLight.outerCutOff"), glm::cos(glm::radians(outerCutOff)));
	}

	~SpotLight() { }
};

#endif