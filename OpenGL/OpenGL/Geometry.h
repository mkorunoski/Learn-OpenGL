#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <glm/glm.hpp>

#include "Mesh.h"

// Constant colors
const glm::vec3 WHITE = glm::vec3(1.0f, 1.0f, 1.0f);
// Constant normals
const glm::vec3 POS_X = glm::vec3(+1.0f, 0.0f, 0.0f);
const glm::vec3 NEG_X = glm::vec3(-1.0f, 0.0f, 0.0f);
const glm::vec3 POS_Y = glm::vec3(0.0f, +1.0f, 0.0f);
const glm::vec3 NEG_Y = glm::vec3(0.0f, -1.0f, 0.0f);
const glm::vec3 POS_Z = glm::vec3(0.0f, 0.0f, +1.0f);
const glm::vec3 NEG_Z = glm::vec3(0.0f, 0.0f, -1.0f);
// Constant texture coordinates
const glm::vec2 TEX_COORDS_ARR[] = { glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f) };

class Geometry
{
public:
	static void GeneratePlane(GLint width, GLint depth, GLuint m, GLuint n, std::vector<Vertex>& vertices, std::vector<GLuint>& indices)
	{
		vertices.clear();
		indices.clear();

		GLuint vertexCount = m * n;
		GLuint faceCount   = 2 * (m - 1) * (n - 1);
		GLfloat halfWidth  = 0.5f * width;
		GLfloat halfDepth  = 0.5f * depth;
		GLfloat dx = (GLfloat)width / (n - 1);
		GLfloat dz = (GLfloat)depth / (m - 1);
		GLfloat du = 1.0f / (n - 1);
		GLfloat dv = 1.0f / (m - 1);

		vertices.resize(vertexCount);
		for (GLuint i = 0; i < m; ++i)
		{
			GLfloat z = halfDepth - i * dz;
			for (GLuint j = 0; j < n; ++j)
			{
				GLfloat x = -halfWidth + j * dx;
				vertices[i * n + j].position  = glm::vec3(x, 0.0f, z);
				vertices[i * n + j].color	  = glm::vec3(1.0f);
				vertices[i * n + j].texCoords = glm::vec2(j * du, i * dv);
				vertices[i * n + j].normal	  = POS_Y;				
			}
		}

		indices.resize(faceCount * 3);
		GLuint k = 0;
		for (GLuint i = 0; i < m - 1; ++i)
		{
			for (GLuint j = 0; j < n - 1; ++j)
			{
				indices[k]	   = i*n + j;
				indices[k + 1] = i*n + j + 1;
				indices[k + 2] = (i + 1)*n + j;
				indices[k + 3] = (i + 1)*n + j;
				indices[k + 4] = i*n + j + 1;
				indices[k + 5] = (i + 1)*n + j + 1;
				k += 6;
			}
		}		
	}

	static void GenerateCube(std::vector<Vertex>& vertices)
	{
		vertices.clear();		

		vertices.resize(36);
		
		glm::vec3 a(-0.5f, -0.5f, -0.5f);
		glm::vec3 b(+0.5f, -0.5f, -0.5f);
		glm::vec3 c(+0.5f, -0.5f, +0.5f);
		glm::vec3 d(-0.5f, -0.5f, +0.5f);
		
		glm::vec3 e(-0.5f, +0.5f, +0.5f);
		glm::vec3 f(+0.5f, +0.5f, +0.5f);
		glm::vec3 g(+0.5f, +0.5f, -0.5f);		
		glm::vec3 h(-0.5f, +0.5f, -0.5f);

		glm::vec3 normal;
		GLuint i = 0;
		//front
		normal = POS_Z;
		vertices[i++] = Vertex(d, normal, WHITE, TEX_COORDS_ARR[0]);
		vertices[i++] = Vertex(c, normal, WHITE, TEX_COORDS_ARR[1]);
		vertices[i++] = Vertex(f, normal, WHITE, TEX_COORDS_ARR[2]);
		vertices[i++] = Vertex(d, normal, WHITE, TEX_COORDS_ARR[0]);
		vertices[i++] = Vertex(f, normal, WHITE, TEX_COORDS_ARR[2]);
		vertices[i++] = Vertex(e, normal, WHITE, TEX_COORDS_ARR[3]);

		//back
		normal = NEG_Z;
		vertices[i++] = Vertex(b, normal, WHITE, TEX_COORDS_ARR[0]);
		vertices[i++] = Vertex(a, normal, WHITE, TEX_COORDS_ARR[1]);
		vertices[i++] = Vertex(h, normal, WHITE, TEX_COORDS_ARR[2]);
		vertices[i++] = Vertex(b, normal, WHITE, TEX_COORDS_ARR[0]);
		vertices[i++] = Vertex(h, normal, WHITE, TEX_COORDS_ARR[2]);
		vertices[i++] = Vertex(g, normal, WHITE, TEX_COORDS_ARR[3]);

		//left
		normal = NEG_X;
		vertices[i++] = Vertex(a, normal, WHITE, TEX_COORDS_ARR[0]);
		vertices[i++] = Vertex(d, normal, WHITE, TEX_COORDS_ARR[1]);
		vertices[i++] = Vertex(e, normal, WHITE, TEX_COORDS_ARR[2]);
		vertices[i++] = Vertex(a, normal, WHITE, TEX_COORDS_ARR[0]);
		vertices[i++] = Vertex(e, normal, WHITE, TEX_COORDS_ARR[2]);
		vertices[i++] = Vertex(h, normal, WHITE, TEX_COORDS_ARR[3]);

		//right
		normal = POS_X;
		vertices[i++] = Vertex(c, normal, WHITE, TEX_COORDS_ARR[0]);
		vertices[i++] = Vertex(b, normal, WHITE, TEX_COORDS_ARR[1]);
		vertices[i++] = Vertex(g, normal, WHITE, TEX_COORDS_ARR[2]);
		vertices[i++] = Vertex(c, normal, WHITE, TEX_COORDS_ARR[0]);
		vertices[i++] = Vertex(g, normal, WHITE, TEX_COORDS_ARR[2]);
		vertices[i++] = Vertex(f, normal, WHITE, TEX_COORDS_ARR[3]);

		//top
		normal = POS_Y;
		vertices[i++] = Vertex(e, normal, WHITE, TEX_COORDS_ARR[0]);
		vertices[i++] = Vertex(f, normal, WHITE, TEX_COORDS_ARR[1]);
		vertices[i++] = Vertex(g, normal, WHITE, TEX_COORDS_ARR[2]);
		vertices[i++] = Vertex(e, normal, WHITE, TEX_COORDS_ARR[0]);
		vertices[i++] = Vertex(g, normal, WHITE, TEX_COORDS_ARR[2]);
		vertices[i++] = Vertex(h, normal, WHITE, TEX_COORDS_ARR[3]);

		//bottom
		normal = NEG_Y;
		vertices[i++] = Vertex(a, normal, WHITE, TEX_COORDS_ARR[0]);
		vertices[i++] = Vertex(b, normal, WHITE, TEX_COORDS_ARR[1]);
		vertices[i++] = Vertex(c, normal, WHITE, TEX_COORDS_ARR[2]);
		vertices[i++] = Vertex(a, normal, WHITE, TEX_COORDS_ARR[0]);
		vertices[i++] = Vertex(c, normal, WHITE, TEX_COORDS_ARR[2]);
		vertices[i++] = Vertex(d, normal, WHITE, TEX_COORDS_ARR[3]);
	}

	~Geometry() { }
};

#endif