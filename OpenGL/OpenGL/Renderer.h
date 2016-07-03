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
#include "CubemapTexture.h"
#include "Material.h"
#include "Light.h"

// Uniform buffer offsets
const GLintptr MODEL_OFFSET		 = 0;
const GLintptr VIEW_OFFSET		 = 1 * sizeof(glm::mat4);
const GLintptr PROJECTION_OFFSET = 2 * sizeof(glm::mat4);
const GLintptr INV_TRANS_OFFSET  = 3 * sizeof(glm::mat4);

// Constats
const GLuint BUFFER_BASE = 0;

class Renderer
{
private:
	Camera* camera;

	// Shaders
	Shader* shader;
	Shader* outliner;
	Shader* skybox;

	// Lights
	DirectionalLight* directionalLight;
	PointLight* pointLight1;
	PointLight* pointLight2;
	PointLight* pointLight3;
	SpotLight* spotLight;

	// Meshes
	Mesh* floor;
	Transformation* floorTransformation;
	Material* floorMaterial;
	Texture* planeTexDiff;
	Texture* planeTexNorm;
	Texture* planeTexSpec;
	Mesh* crate;
	Transformation* crateTransformation;
	Material* crateMaterial;
	Texture* crateTexDiff;
	Texture* crateTexSpec;

	Transformation* skyboxTransformation;
	CubemapTexture* skyboxTex;

	// Uniform buffers and uniform locations
	GLuint UBO;
	GLuint uEyePosition;
	GLuint uWhichLight;
	GLuint uNormalFromMap;
	GLuint uReflection;

public:
	// For animation	
	GLfloat angle = 0.0f;
	int whichLight = 1;
	bool outlineCube = false;

public:
	void IncrementAngle(GLfloat deltaTime) { angle += deltaTime; }
	void ChangeOutlineCube() { outlineCube = outlineCube ? false : true; }

	Renderer(Camera* camera, GLfloat wndWidth, GLfloat wndHeight)
	{		
		this->camera = camera;

		shader   = new Shader("./res/shaders/shader.vs", "./res/shaders/shader.fs");
		outliner = new Shader("./res/shaders/outliner.vs", "./res/shaders/outliner.fs");
		skybox	 = new Shader("./res/shaders/skybox.vs", "./res/shaders/skybox.fs");

		directionalLight = new DirectionalLight(
			shader->GetProgram(),
			glm::vec4(0.4f, 0.4f, 0.4f, 1.0f),
			glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
			glm::vec4(1.0f),
			glm::vec4(0.0f, 10.0f, 10.0f, 1.0f));

		pointLight1 = new PointLight(
			shader->GetProgram(), 0,
			glm::vec4(0.4f, 0.4f, 0.4f, 1.0f),
			glm::vec4(0.2f, 1.0f, 1.0f, 1.0f),
			glm::vec4(1.0f),
			glm::vec4(10.0f, 1.0f, 0.0f, 1.0f),
			1.0f, 0.07f, 0.017f);
		pointLight2 = new PointLight(
			shader->GetProgram(), 1,
			glm::vec4(0.4f, 0.4f, 0.4f, 1.0f),
			glm::vec4(1.0f, 0.2f, 1.0f, 1.0f),
			glm::vec4(1.0f),
			glm::vec4(-10.0f, 1.0f, 0.0f, 1.0f),
			1.0f, 0.07f, 0.017f);
		pointLight3 = new PointLight(
			shader->GetProgram(), 2,
			glm::vec4(0.4f, 0.4f, 0.4f, 1.0f),
			glm::vec4(1.0f, 1.0f, 0.2f, 1.0f),
			glm::vec4(1.0f),
			glm::vec4(0.0f, 1.0f, -10.0f, 1.0f),
			1.0f, 0.07f, 0.017f);

		spotLight = new SpotLight(
			shader->GetProgram(),
			glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),
			glm::vec4(1.0f),
			glm::vec4(1.0f),
			glm::vec4(0.0f, 5.0f, 20.0f, 1.0f),
			glm::vec4(0.0f, 0.0f, -1.0f, 0.0f),
			17.5f, 12.5f);

		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;

		Geometry::GeneratePlane(50, 50, 5, 5, vertices, indices);
		floor = new Mesh(vertices, vertices.size(), indices, indices.size());
		floorTransformation = new Transformation();
		floorMaterial = new Material(
			glm::vec4(1.0f),
			glm::vec4(1.0f),
			glm::vec4(1.0f),
			64);
		planeTexDiff = new Texture("./res/textures/floor/diffuse.jpg");
		planeTexNorm = new Texture("./res/textures/floor/normal.jpg");
		planeTexSpec = new Texture("./res/textures/floor/specular.jpg");

		Geometry::GenerateCube(vertices);
		crate = new Mesh(vertices, vertices.size());
		crateTransformation = new Transformation();						
		crateMaterial = new Material(
			glm::vec4(1.0f),
			glm::vec4(1.0f),
			glm::vec4(1.0f),
			8);
		crateTexDiff = new Texture("./res/textures/crate/diffuse.jpg");
		crateTexSpec = new Texture("./res/textures/crate/specular.jpg");

		// skyboxTransformation = new Transformation();
		// skyboxTex = new CubemapTexture("./res/textures/cubemaps/", "jpg");
		
		glm::mat4 projection = glm::perspective(70.0f, wndWidth / wndHeight, 0.1f, 1000.0f);
		
		glGenBuffers(1, &UBO);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferBase(GL_UNIFORM_BUFFER, BUFFER_BASE, UBO);

		GLuint uTransformationBlockS = glGetUniformBlockIndex(shader->GetProgram(), "TransformationBlock");
		glUniformBlockBinding(shader->GetProgram(), uTransformationBlockS, BUFFER_BASE);
		GLuint uTransformationBlockO = glGetUniformBlockIndex(outliner->GetProgram(), "TransformationBlock");
		glUniformBlockBinding(outliner->GetProgram(), uTransformationBlockO, BUFFER_BASE);
		
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, PROJECTION_OFFSET, sizeof(glm::mat4), glm::value_ptr(projection));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		uEyePosition   = glGetUniformLocation(shader->GetProgram(), "eyePosition");
		uWhichLight    = glGetUniformLocation(shader->GetProgram(), "whichLight");
		uNormalFromMap = glGetUniformLocation(shader->GetProgram(), "normalFromMap");
		uReflection	   = glGetUniformLocation(shader->GetProgram(), "reflection");
	}

	void ActivateLights(Shader* shader)
	{
		shader->Use();			
			glUniform3fv(uEyePosition, 1, glm::value_ptr(camera->GetEyePos()));
			glUniform1i(uWhichLight, whichLight);			
			directionalLight->Use();
			pointLight1->Use();
			pointLight2->Use();
			pointLight3->Use();
			spotLight->position = glm::vec4(camera->GetEyePos(), 1.0f);
			spotLight->direction = glm::vec4(camera->GetFront(), 0.0f);
			spotLight->Use();
		shader->Unuse();
	}

	void ActivateLightsInvertY(Shader* shader)
	{
		directionalLight->InvertY();
		pointLight1->InvertY();
		pointLight2->InvertY();
		pointLight3->InvertY();	
		ActivateLights(shader);		
		pointLight1->InvertY();
		pointLight2->InvertY();
		pointLight3->InvertY();
		directionalLight->InvertY();
	}

	void DrawFloor(Shader* shader)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, MODEL_OFFSET, sizeof(glm::mat4), glm::value_ptr(floorTransformation->GetModel()));
		glBufferSubData(GL_UNIFORM_BUFFER, INV_TRANS_OFFSET, sizeof(glm::mat4), glm::value_ptr(floorTransformation->GetInverseTranspose()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		shader->Use();
			glUniform1i(uNormalFromMap, 0);
			floorMaterial->Use(shader->GetProgram());
			planeTexDiff->Use(shader->GetProgram(), "maps.diffuse", 0);
			planeTexNorm->Use(shader->GetProgram(), "maps.normal", 1);
			planeTexSpec->Use(shader->GetProgram(), "maps.specular", 2);
			floor->DrawElements();
			planeTexSpec->Unuse();
			planeTexNorm->Unuse();
			planeTexDiff->Unuse();
		shader->Unuse();
	}

	void DrawOutline(Shader* shader)
	{	
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, MODEL_OFFSET, sizeof(glm::mat4), glm::value_ptr(crateTransformation->GetModel()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);		
		shader->Use();
			crate->DrawArrays();
		shader->Unuse();
	}

	void DrawCrate(Shader* shader)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, MODEL_OFFSET, sizeof(glm::mat4), glm::value_ptr(crateTransformation->GetModel()));
		glBufferSubData(GL_UNIFORM_BUFFER, INV_TRANS_OFFSET, sizeof(glm::mat4), glm::value_ptr(crateTransformation->GetInverseTranspose()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		shader->Use();
			glUniform1i(uNormalFromMap, 0);
			crateMaterial->Use(shader->GetProgram());
			crateTexDiff->Use(shader->GetProgram(), "maps.diffuse", 0);
			crateTexSpec->Use(shader->GetProgram(), "maps.specular", 1);
			crate->DrawArrays();
			crateTexSpec->Unuse();
			crateTexDiff->Unuse();
		shader->Unuse();
	}

	void DrawSkybox(Shader* shader)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, MODEL_OFFSET, sizeof(glm::mat4), glm::value_ptr(skyboxTransformation->GetModel()));
		glBufferSubData(GL_UNIFORM_BUFFER, INV_TRANS_OFFSET, sizeof(glm::mat4), glm::value_ptr(skyboxTransformation->GetInverseTranspose()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		shader->Use();		
		skyboxTex->Use();
		crate->DrawArrays();		
		skyboxTex->Unuse();
		shader->Unuse();
	}

	void RenderScene()
	{	
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, VIEW_OFFSET, sizeof(glm::mat4), glm::value_ptr(camera->GetViewMatrix()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		
		crateTransformation->Rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));

		// Crate reflection	
		if (true)
		{	
			shader->Use();
			glUniform1i(uReflection, true);
			shader->Unuse();

			ActivateLightsInvertY(shader);
			
			glEnable(GL_STENCIL_TEST);
			glClear(GL_STENCIL_BUFFER_BIT);

			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glDepthMask(GL_FALSE);
			glStencilMask(0xFF); // enable writing to stencil buffer
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			DrawFloor(shader);

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glDepthMask(GL_TRUE);
			glStencilMask(0x00);
			glStencilFunc(GL_EQUAL, 1, 0xFF);
			crateTransformation->Scale(glm::vec3(5.0f, 5.0f, 5.0f));
			crateTransformation->Translate(glm::vec3(0.0f, -5.0f, 0.0f));			
			DrawCrate(shader);

			glDisable(GL_STENCIL_TEST);
		}

		ActivateLights(shader);

		shader->Use();
		glUniform1i(uReflection, false);
		shader->Unuse();		

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		DrawFloor(shader);
		glDisable(GL_BLEND);

		if (outlineCube)
		{
			// Outline
			crateTransformation->Scale(glm::vec3(5.2f, 5.2f, 5.2f));
			crateTransformation->Translate(glm::vec3(0.0f, 5.0f, 0.0f));			
			DrawOutline(outliner);			
		}
		
		// Skybox
		// glFrontFace(GL_CW);	
		// skyboxTransformation->Scale(glm::vec3(500.0f, 500.0f, 500.0f));
		// DrawSkybox(skybox);
		// glFrontFace(GL_CCW);

		// Crate
		glDisable(GL_DEPTH_TEST);
		crateTransformation->Scale(glm::vec3(5.0f, 5.0f, 5.0f));
		crateTransformation->Rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));
		crateTransformation->Translate(glm::vec3(0.0f, 5.0f, 0.0f));
		DrawCrate(shader);
		glEnable(GL_DEPTH_TEST);
	}

	~Renderer()
	{
		delete(shader);
		delete(outliner);

		delete(directionalLight);
		delete(pointLight1);
		delete(pointLight2);
		delete(spotLight);

		delete(floor);
		delete(floorTransformation);
		delete(floorMaterial);
		delete(planeTexDiff);
		delete(planeTexNorm);
		delete(planeTexSpec);
		delete(crate);
		delete(crateTransformation);
		delete(crateMaterial);
		delete(crateTexDiff);
		delete(crateTexSpec);
	}
};

#endif