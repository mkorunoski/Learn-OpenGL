#ifndef MESH_H
#define MESH_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

const GLuint POSITION_LENGTH	= 3;
const GLuint NORMAL_LENGTH		= 3;
const GLuint COLOR_LENGTH		= 3;
const GLuint TEX_COORDS_LENGTH	= 2;
const GLuint VERTEX_LENGTH		= POSITION_LENGTH + NORMAL_LENGTH + COLOR_LENGTH + TEX_COORDS_LENGTH;

enum ATTRIBUTE_LOCATION
{
	POSITION = 0,
	NORMAL,
	COLOR,
	TEX_COORDS
};

typedef struct Vertex
{	
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texCoords;

	Vertex()
	{
		this->position	= glm::vec3(0.0f);
		this->normal	= glm::vec3(0.0f);
		this->color		= glm::vec3(1.0f);
		this->texCoords = glm::vec2(0.0f);
	}

	Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& color, const glm::vec2& texCoords)
	{
		this->position	= position;
		this->normal	= normal;
		this->color		= color;
		this->texCoords = texCoords;
	}
} Vertex;

class Mesh
{
private:
	GLuint VAO;
	GLuint VBO, EBO;
	GLuint numIndices;
	GLuint numVertices;
	
	void AttributePointers()
	{
		glVertexAttribPointer(ATTRIBUTE_LOCATION::POSITION,
			POSITION_LENGTH,
			GL_FLOAT, GL_FALSE,
			VERTEX_LENGTH * sizeof(GLfloat),
			(const GLvoid*)offsetof(Vertex, position));
		glEnableVertexAttribArray(ATTRIBUTE_LOCATION::POSITION);

		glVertexAttribPointer(ATTRIBUTE_LOCATION::NORMAL,
			NORMAL_LENGTH,
			GL_FLOAT, GL_FALSE,
			VERTEX_LENGTH * sizeof(GLfloat),
			(const GLvoid*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(ATTRIBUTE_LOCATION::NORMAL);

		glVertexAttribPointer(ATTRIBUTE_LOCATION::COLOR,
			COLOR_LENGTH,
			GL_FLOAT, GL_FALSE,
			VERTEX_LENGTH * sizeof(GLfloat),
			(const GLvoid*)offsetof(Vertex, color));
		glEnableVertexAttribArray(ATTRIBUTE_LOCATION::COLOR);

		glVertexAttribPointer(ATTRIBUTE_LOCATION::TEX_COORDS,
			TEX_COORDS_LENGTH,
			GL_FLOAT, GL_FALSE,
			VERTEX_LENGTH * sizeof(GLfloat),
			(const GLvoid*)offsetof(Vertex, texCoords));
		glEnableVertexAttribArray(ATTRIBUTE_LOCATION::TEX_COORDS);
	}

public:
	Mesh() { }

	Mesh(const std::vector<Vertex>& vertices, GLuint numVertices)
	{
		this->numVertices = numVertices;

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		
		AttributePointers();

		glBindVertexArray(0);
	}

	Mesh(const std::vector<Vertex>& vertices, GLuint numVertices, const std::vector<GLuint>& indices, GLuint numIndices)
	{
		this->numIndices = numIndices;
		this->numVertices = numVertices;

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

		AttributePointers();

		glBindVertexArray(0);
	}

	void DrawElements()
	{
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void DrawArrays()
	{
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, numVertices);
		glBindVertexArray(0);
	}

	~Mesh()
	{
		glDeleteBuffers(1, &EBO);
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);		
	}
};

#endif