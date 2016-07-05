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

enum UniformLoc
{
	MODEL = 10,	
	INVERSE_TRANSPOSE,
	EYE_POSITION = 20,
	SKYBOX_TEX = 30,
};

class Renderer
{
private:
	GLuint wndWidth;
	GLuint wndHeight;
	Camera* camera;	

	// Shaders
	static const GLuint NUM_SHADERS = 3;
	Shader defaultShader;
	Shader skybox;
	Shader reflRefr;
	
	// Lights
	static const GLuint NUM_POINT_LIGHTS = 3;
	DirectionalLight directionalLight;	
	PointLight pointLights[NUM_POINT_LIGHTS];
	SpotLight spotLight;
	
	// Meshes
	Mesh cube;
	Mesh plane;
	Mesh loadedMesh;

	Material cubeMaterial;	
	Material planeMaterial;
	Material loadedMeshMaterial;

	Transformation cubeTransformation;
	Transformation planeTransformation;	
	Transformation loadedMeshTransformation;
	Transformation skyboxTransformation;

	Texture checkered;
	Texture marble;
	Texture waterNormalMap;
	CubemapTexture skyboxTex;

	GLuint UBO;
	
public:
	// For animation	
	GLfloat angle = 0.0f;	

private:
	void CompileShaders()
	{
		defaultShader = Shader("./res/shaders/shader.vs", "./res/shaders/shader.fs");
		skybox		  = Shader("./res/shaders/skybox.vs", "./res/shaders/skybox.fs");
		reflRefr	  = Shader("./res/shaders/reflective_refractive.vs", "./res/shaders/reflective_refractive.fs");
	}

	void SetupLights()
	{
		directionalLight = DirectionalLight(
			glm::vec3(0.1f), glm::vec3(0.5f), glm::vec3(0.5f),
			glm::vec3(-10.0f, 50.0f, -10.0f));
		
		pointLights[0] = PointLight(
			glm::vec3(0.1f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.01f),
			glm::vec3(10.0f, 1.0f, 0.0f),
			1.0f, 0.07f, 0.017f);
		pointLights[1] = PointLight(
			glm::vec3(0.1f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.01f),
			glm::vec3(-10.0f, 1.0f, 0.0f),
			1.0f, 0.07f, 0.017f);
		pointLights[2] = PointLight(
			glm::vec3(0.1f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.01f),
			glm::vec3(0.0f, 1.0f, 10.0f),
			1.0f, 0.07f, 0.017f);

		spotLight = SpotLight(
			glm::vec3(0.1f), glm::vec3(1.0f), glm::vec3(1.0f),
			glm::vec3(0.0f, 5.0f, 20.0f),
			glm::vec3(0.0f, 0.0f, -1.0f),
			17.5f, 12.5f);
	}

	void AcivateLights(Shader& shader)
	{
		directionalLight.SetUniforms(shader.GetProgram());
		for (GLuint i = 0; i < NUM_POINT_LIGHTS; ++i)
			pointLights[i].SetUniforms(shader.GetProgram(), i);
		spotLight.SetUniforms(shader.GetProgram());
		shader.Use();
			directionalLight.Use();
			for (int i = 0; i < NUM_POINT_LIGHTS; ++i)
				pointLights[i].Use();
			spotLight.SetPosition(camera->GetEyePos());
			spotLight.SetDirection(camera->GetFront());
			spotLight.Use();
		shader.Unuse();
	}

	void LoadMeshes()
	{
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;

		Geometry::GenerateCube(vertices);
		cube		= Mesh(vertices);
		Geometry::GeneratePlane(50, 50, 10, 10, vertices, indices);
		plane		= Mesh(vertices, indices);
		Geometry::GenerateFromFile("./res/objects/sphere.obj", vertices, indices);
		loadedMesh	= Mesh(vertices, indices);

		cubeTransformation		 = Transformation();
		planeTransformation		 = Transformation();
		loadedMeshTransformation = Transformation();		
		
		cubeMaterial		 = Material(glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 64);
		planeMaterial		 = Material(glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 128);
		loadedMeshMaterial	 = Material(glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 2);
		skyboxTransformation = Transformation();

		checkered = Texture("./res/textures/checkered.jpg");
		marble	  = Texture("./res/textures/marble.jpg");
		waterNormalMap = Texture("./res/textures/water_nm.jpg");
		skyboxTex = CubemapTexture("./res/textures/cubemaps/", "jpg");
	}

	void SetupUniformBufferObjects()
	{
		const GLuint BINDING_POINT0 = 0;
		GLuint UBIndices[NUM_SHADERS];
		const GLchar* UB_NAME = "ViewProjection";
		const GLuint PROGRAMS[NUM_SHADERS] = { defaultShader.GetProgram(), skybox.GetProgram(), reflRefr.GetProgram() };  // add the reference to the new shader's program here
		GLuint i = 0;

		glGenBuffers(1, &UBO);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT0, UBO);
		
		for (i = 0; i < NUM_SHADERS; ++i)
		{
			UBIndices[i] = glGetUniformBlockIndex(PROGRAMS[i], UB_NAME);
			glUniformBlockBinding(PROGRAMS[i], UBIndices[i], BINDING_POINT0);
		}

		glm::mat4 projection = glm::perspective(70.0f, (GLfloat)wndWidth / (GLfloat)wndHeight, 0.1f, 1000.0f);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

public:
	void IncrementAngle(GLfloat deltaTime) { angle += deltaTime; }

	Renderer(Camera* camera, GLuint wndWidth, GLuint wndHeight)
	{		
		this->camera = camera;
		this->wndWidth = wndWidth;
		this->wndHeight = wndHeight;

		CompileShaders();
		SetupLights();
		LoadMeshes();
		SetupUniformBufferObjects();
	}
	
	void RenderScene()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera->GetViewMatrix()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// Skybox		
		skyboxTransformation.Scale(glm::vec3(500.0f));
		skyboxTransformation.Translate(camera->GetEyePos());
		skybox.Use();
			glUniformMatrix4fv(UniformLoc::MODEL, 1, false, glm::value_ptr(skyboxTransformation.GetModel()));			
			glFrontFace(GL_CW);
			skyboxTex.Use();
			cube.DrawArrays();
			skyboxTex.Unuse();
			glFrontFace(GL_CCW);
		skybox.Unuse();

		// Lights
		AcivateLights(defaultShader);

		// Center sphere
		defaultShader.Use();			
			glUniform3fv(UniformLoc::EYE_POSITION, 1, glm::value_ptr(camera->GetEyePos()));
		defaultShader.Use();				
		loadedMeshTransformation.Scale(glm::vec3(50.0f));
		loadedMeshTransformation.Translate(glm::vec3(0.0f, 5.0f, 0.0f));
		defaultShader.Use();
			glUniformMatrix4fv(UniformLoc::MODEL, 1, false, glm::value_ptr(loadedMeshTransformation.GetModel()));
			glUniformMatrix4fv(UniformLoc::INVERSE_TRANSPOSE, 1, false, glm::value_ptr(loadedMeshTransformation.GetInverseTranspose()));						
			cubeMaterial.Use(defaultShader.GetProgram());
			marble.Use(defaultShader.GetProgram(), "maps.diffuse", 0);
			loadedMesh.DrawElements();
			marble.Unuse();
		defaultShader.Unuse();	

		// Reflective/Refractive spheres
		reflRefr.Use();			
			glUniform3fv(UniformLoc::EYE_POSITION, 1, glm::value_ptr(camera->GetEyePos()));
		reflRefr.Use();
		for (GLfloat i = -10.0f; i <= 10.0f; i += 20.0f)
		{			
			loadedMeshTransformation.Scale(glm::vec3(50.0f));
			loadedMeshTransformation.Translate(glm::vec3(i, 5.0f, 0.0f));
			reflRefr.Use();
				glUniformMatrix4fv(UniformLoc::MODEL, 1, false, glm::value_ptr(loadedMeshTransformation.GetModel()));
				glUniformMatrix4fv(UniformLoc::INVERSE_TRANSPOSE, 1, false, glm::value_ptr(loadedMeshTransformation.GetInverseTranspose()));
				skyboxTex.Use();
				cubeMaterial.Use(reflRefr.GetProgram());
				loadedMesh.DrawElements();
				skyboxTex.Unuse();
			reflRefr.Unuse();
		}

		// Plane
		defaultShader.Use();
			glUniform3fv(UniformLoc::EYE_POSITION, 1, glm::value_ptr(camera->GetEyePos()));
		defaultShader.Unuse();
		planeTransformation.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
		defaultShader.Use();
			glUniformMatrix4fv(UniformLoc::MODEL, 1, false, glm::value_ptr(planeTransformation.GetModel()));
			glUniformMatrix4fv(UniformLoc::INVERSE_TRANSPOSE, 1, false, glm::value_ptr(planeTransformation.GetInverseTranspose()));
			planeMaterial.Use(defaultShader.GetProgram());
			checkered.Use(defaultShader.GetProgram(), "maps.diffuse", 0);		
			plane.DrawElements();
			checkered.Unuse();
		defaultShader.Unuse();
	}

	~Renderer() { }
};

#endif