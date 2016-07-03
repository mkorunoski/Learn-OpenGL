#ifndef MATERIAL_H
#define MATERIAL_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Material
{	
private:
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	GLfloat shininess;

public:
	Material(const glm::vec4& ambient, const glm::vec4& diffuse, const glm::vec4& specular, GLfloat shininess)
	{
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->shininess = shininess;
	}

	void Use(const GLuint& program)
	{
		glUniform4fv(glGetUniformLocation(program, "material.ambient"), 1, glm::value_ptr(ambient));
		glUniform4fv(glGetUniformLocation(program, "material.diffuse"), 1, glm::value_ptr(diffuse));
		glUniform4fv(glGetUniformLocation(program, "material.specular"), 1, glm::value_ptr(specular));
		glUniform1f(glGetUniformLocation(program, "material.shininess"), shininess);
	}

	~Material() { }
};

#endif