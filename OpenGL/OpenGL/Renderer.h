#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Mesh.h"
#include "Geometry.h"
#include "Transformation.h"
#include "Texture.h"
#include "Material.h"
#include "Light.h"

const GLintptr MODEL_OFFSET		 = 0;
const GLintptr VIEW_OFFSET		 = 1 * sizeof(glm::mat4);
const GLintptr PROJECTION_OFFSET = 2 * sizeof(glm::mat4);
const GLintptr INV_TRANS_OFFSET  = 3 * sizeof(glm::mat4);

class Renderer
{
public:
	Shader* shader;

	DirectionalLight* directionalLight;
	PointLight* pointLight;
	SpotLight* spotLight;

	Mesh* plane;	
	Transformation* planeTransformation;
	Material* planeMaterial;
	Texture* checkeredTexture;	
	Texture* checkeredSpecularLMTexture;

	Mesh* crate;
	Transformation* crateTransformation;
	Material* crateMaterial;
	Texture* crateTexture;
	Texture* crateSpecularLMTexture;

	GLuint UBO;
	
	Renderer(GLfloat wndWidth, GLfloat wndHeight)
	{		
		shader = new Shader("./res/shaders/shader.vs", "./res/shaders/shader.gs", "./res/shaders/shader.fs");

		directionalLight = new DirectionalLight(
			glm::vec4(0.01f, 0.01f, 0.01f, 1.0f),
			glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
			glm::vec4(1.0f),
			glm::vec4(0.0f, 10.0f, 10.0f, 1.0f));

		pointLight = new PointLight(
			glm::vec4(0.01f, 0.01f, 0.01f, 1.0f),
			glm::vec4(0.2f, 0.8f, 0.8f, 1.0f),
			glm::vec4(1.0f),
			glm::vec4(6.0f, 1.0f, 0.0f, 1.0f),
			1.0f,
			0.07f,
			0.017f);

		spotLight = new SpotLight(
			glm::vec4(0.01f, 0.01f, 0.01f, 1.0f),
			glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
			glm::vec4(1.0f),
			glm::vec4(0.0f, 5.0f, 20.0f, 1.0f),
			glm::vec4(0.0f, 0.0f, -1.0f, 0.0f),
			17.5f,
			12.5f);

		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;

		Geometry::GeneratePlane(50, 50, 5, 5, vertices, indices);
		plane = new Mesh(vertices, vertices.size(), indices, indices.size());
		planeTransformation = new Transformation();
		planeMaterial = new Material(
			glm::vec4(1.0f),
			glm::vec4(1.0f),
			glm::vec4(1.0f),
			64);
		checkeredTexture = new Texture("./res/textures/checkered.jpg");
		checkeredSpecularLMTexture = new Texture("./res/textures/stains_specular_lm.jpg");

		Geometry::GenerateCube(vertices);
		crate = new Mesh(vertices, vertices.size());
		crateTransformation = new Transformation();		
		crateTransformation->Scale(glm::vec3(5.0f, 5.0f, 5.0f));
		crateTransformation->Translate(glm::vec3(0.0f, 5.0f, 0.0f));
		crateMaterial = new Material(
			glm::vec4(1.0f),
			glm::vec4(1.0f),
			glm::vec4(1.0f),
			8);
		crateTexture = new Texture("./res/textures/crate.png");
		crateSpecularLMTexture = new Texture("./res/textures/crate_specular_lm.jpg");
				
		glm::mat4 projection = glm::perspective(70.0f, wndWidth / wndHeight, 0.1f, 1000.0f);
		shader->Use();
		GLuint uTransformationBlock = glGetUniformBlockIndex(shader->program, "TransformationBlock");
		glUniformBlockBinding(shader->program, uTransformationBlock, 0);
		
		glGenBuffers(1, &UBO);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
		
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, PROJECTION_OFFSET, sizeof(glm::mat4), glm::value_ptr(projection));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		shader->Unuse();
	}

	GLfloat angle = 0.0f;

	void RenderScene(Camera& camera, int numLights, GLfloat deltaTime)
	{
		shader->Use();
		
		glUniform3fv(glGetUniformLocation(shader->program, "eyePos"), 1, glm::value_ptr(camera.GetEyePos()));
		glUniform1i(glGetUniformLocation(shader->program, "numLights"), numLights);
		
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, VIEW_OFFSET, sizeof(glm::mat4), glm::value_ptr(camera.GetViewMatrix()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		
		directionalLight->Use(shader->program);
		pointLight->Use(shader->program);		

		spotLight->position  = glm::vec4(camera.Position, 1.0f);
		spotLight->direction = glm::vec4(camera.Front, 0.0f);
		spotLight->Use(shader->program);

		//plane
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, MODEL_OFFSET, sizeof(glm::mat4), glm::value_ptr(planeTransformation->GetModel()));
		glBufferSubData(GL_UNIFORM_BUFFER, INV_TRANS_OFFSET, sizeof(glm::mat4), glm::value_ptr(planeTransformation->GetInverseTranspose()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		planeMaterial->Use(shader->program);
		checkeredTexture->Use(shader->program, "texture", 0);
		checkeredSpecularLMTexture->Use(shader->program, "specularLM", 1);
		plane->DrawElements();		
		checkeredSpecularLMTexture->Unuse();
		checkeredTexture->Unuse();
				
		//crate
		angle += deltaTime;
		crateTransformation->Rotate(angle, glm::vec3(1.0f, 1.0f, 1.0f));
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, MODEL_OFFSET, sizeof(glm::mat4), glm::value_ptr(crateTransformation->GetModel()));
		glBufferSubData(GL_UNIFORM_BUFFER, INV_TRANS_OFFSET, sizeof(glm::mat4), glm::value_ptr(crateTransformation->GetInverseTranspose()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		crateMaterial->Use(shader->program);
		crateTexture->Use(shader->program, "texture", 0);
		crateSpecularLMTexture->Use(shader->program, "specularLM", 1);
		crate->DrawArrays();
		crateTexture->Unuse();
		crateSpecularLMTexture->Unuse();
		shader->Unuse();		
	}

	~Renderer() { }
};

#endif