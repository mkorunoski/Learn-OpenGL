#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Transformation
{
private:
	glm::mat4 scale;
	glm::mat4 rotate;
	glm::mat4 translate;

	glm::mat4 modelMatrix;

	void UpdateModelMatrix() { modelMatrix = translate * rotate * scale; }
public:
	Transformation()
	{
		scale	  = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
		rotate	  = glm::rotate(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		translate = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
		UpdateModelMatrix();
	}

	void Scale(const glm::vec3& v)
	{
		scale = glm::scale(v);
		UpdateModelMatrix();
	}

	void Rotate(GLfloat a, const glm::vec3& v)
	{
		rotate = glm::rotate(a, v);
		UpdateModelMatrix();
	}

	void Translate(const glm::vec3& v)
	{
		translate = glm::translate(v);
		UpdateModelMatrix();
	}

	glm::mat4& GetModel() { return modelMatrix; }
	glm::mat4 GetInverseTranspose() { return glm::transpose(glm::inverse(modelMatrix)); }

	~Transformation() { }
};

#endif