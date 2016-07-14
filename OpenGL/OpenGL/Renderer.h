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
	static const GLuint NUM_SHADERS = 4;
	Shader defaultShader;
	Shader defaultShaderNM;
	Shader skyboxShader;
	Shader reflRefrShader;
	
	// Lights
	static const GLuint NUM_POINT_LIGHTS = 3;
	DirectionalLight directionalLight;	
	PointLight pointLights[NUM_POINT_LIGHTS];
	SpotLight spotLight;
	
	// Meshes
	Mesh cubeMesh;
	Mesh planeMesh;
	Mesh loadedMesh;

	Material cubeMaterial;	
	Material planeMaterial;
	Material wallMaterial;
	Material loadedMeshMaterial;

	Transformation cubeTransformation;
	Transformation planeTransformation;	
	Transformation loadedMeshTransformation;
	Transformation skyboxTransformation;

	Texture checkeredTex;
	Texture marbleTex;
	Texture waterDiffuseTex;
	Texture waterNormalTex;
	Texture waterBumpTex;
	Texture wallDiffuseTex;
	Texture wallNormalTex;	
	CubemapTexture skyboxTex;

	GLuint UBO;
	
public:
	// For animation	
	GLfloat dt	= 0.0f;	
	GLfloat A	= 50.0f;
	GLfloat t	= 0.5f;

private:
	void CompileShaders()
	{
		defaultShader	= Shader("./res/shaders/default_shader.vs", "./res/shaders/default_shader.fs", "default_shader");
		defaultShaderNM = Shader("./res/shaders/default_shader_nm.vs", "./res/shaders/default_shader_nm.fs", "default_shader_nm");
		skyboxShader	= Shader("./res/shaders/skybox.vs", "./res/shaders/skybox.fs", "skybox");
		reflRefrShader	= Shader("./res/shaders/reflective_refractive.vs", "./res/shaders/reflective_refractive.fs", "reflective_refractive");
	}

	void SetupLights()
	{
		directionalLight = DirectionalLight(
			glm::vec3(0.05f), glm::vec3(0.4f), glm::vec3(0.4f),
			glm::vec3(0.0f, 25.0f, 0.0f));
		
		pointLights[0] = PointLight(
			glm::vec3(0.1f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.01f),
			glm::vec3(10.0f, 1.0f, 0.0f),
			1.0f, 0.07f, 0.017f);
		pointLights[1] = PointLight(
			glm::vec3(0.1f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.01f),
			glm::vec3(-10.0f, 1.0f, 0.0f),
			1.0f, 0.07f, 0.017f);
		pointLights[2] = PointLight(
			glm::vec3(0.1f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.01f),
			glm::vec3(0.0f, 1.0f, 10.0f),
			1.0f, 0.07f, 0.017f);

		spotLight = SpotLight(
			glm::vec3(0.1f), glm::vec3(1.0f), glm::vec3(1.0f),
			glm::vec3(0.0f, 5.0f, 20.0f),
			glm::vec3(0.0f, 0.0f, -1.0f),
			17.5f, 12.5f);
	}

	void ActivateDirectionalLights(Shader& shader)
	{
		directionalLight.SetUniforms(shader.GetProgram());
		shader.Use();
			directionalLight.Use();
		shader.Unuse();
	}
	void ActivatePointLights(Shader& shader)
	{
		for (GLuint i = 0; i < NUM_POINT_LIGHTS; ++i)
			pointLights[i].SetUniforms(shader.GetProgram(), i);
		shader.Use();
			for (int i = 0; i < NUM_POINT_LIGHTS; ++i)
				pointLights[i].Use();
		shader.Unuse();
	}
	void ActivateSpotLight(Shader& shader)
	{
		spotLight.SetUniforms(shader.GetProgram());
		shader.Use();
			spotLight.SetPosition(camera->GetEyePos());
			spotLight.SetDirection(camera->GetFront());
			spotLight.Use();
		shader.Unuse();
	}

	void AcivateLights(Shader& shader)
	{
		ActivateDirectionalLights(shader);
		ActivatePointLights(shader);		
		ActivateSpotLight(shader);
	}

	void LightsInvertY()
	{
		directionalLight.InvertY();
		for (GLuint i = 0; i < NUM_POINT_LIGHTS; ++i)
			pointLights[i].InvertY();
	}

	void LoadMeshes()
	{
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;

		Geometry::GenerateCube(vertices);
		cubeMesh	= Mesh(vertices);
		Geometry::GeneratePlane(30, 30, 2, 2, vertices, indices);
		planeMesh	= Mesh(vertices, indices);
		Geometry::GenerateFromFile("./res/objects/sphere.obj", vertices, indices);
		loadedMesh	= Mesh(vertices, indices);

		cubeTransformation		 = Transformation();
		planeTransformation		 = Transformation();
		loadedMeshTransformation = Transformation();		
		skyboxTransformation	 = Transformation();
		
		cubeMaterial		 = Material(glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 2);
		planeMaterial		 = Material(glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 64);
		wallMaterial		 = Material(glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 2);
		loadedMeshMaterial	 = Material(glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 64);		

		checkeredTex	= Texture("./res/textures/checkered.jpg");
		marbleTex		= Texture("./res/textures/marble.jpg");
		waterNormalTex	= Texture("./res/textures/water/normal.jpg");
		waterDiffuseTex	= Texture("./res/textures/water/diffuse.jpg");
		wallNormalTex	= Texture("./res/textures/wall/normal.jpg");
		wallDiffuseTex	= Texture("./res/textures/wall/diffuse.jpg");
		waterBumpTex	= Texture("./res/textures/water/bump.jpg");
		skyboxTex		= CubemapTexture("./res/textures/cubemaps/", "jpg");		
	}

	void SetupUniformBufferObjects()
	{
		const GLuint BINDING_POINT0 = 0;
		GLuint UBIndices[NUM_SHADERS];
		const GLchar* UB_NAME = "ViewProjection";
		const GLuint PROGRAMS[NUM_SHADERS] = { defaultShader.GetProgram(), defaultShaderNM.GetProgram(), skyboxShader.GetProgram(), reflRefrShader.GetProgram() };  // add the reference to the new shader's program here
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
	void IncrementAngle(GLfloat deltaTime) { dt += deltaTime; }

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
		directionalLight.Move(A * glm::sin(t * dt), A * glm::cos(t * dt));

		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera->GetViewMatrix()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
				
#pragma region Floor reflection
		// Draw to stencil buffer
		glEnable(GL_STENCIL_TEST);
		glDisable(GL_DEPTH_TEST);

		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);		

		// Floor
		planeTransformation.Rotate(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		planeTransformation.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
		defaultShader.Use();
			glUniformMatrix4fv(UniformLoc::MODEL, 1, false, glm::value_ptr(planeTransformation.GetModel()));
			glUniformMatrix4fv(UniformLoc::INVERSE_TRANSPOSE, 1, false, glm::value_ptr(planeTransformation.GetInverseTranspose()));
			planeMesh.DrawElements();
		defaultShader.Unuse();

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
		glStencilMask(0x00);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

		// Reflection
		RenderReflection();

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
#pragma endregion Floor reflection

		// Lights
		AcivateLights(defaultShader);

		// Floor
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		defaultShader.Use();
			glUniform3fv(UniformLoc::EYE_POSITION, 1, glm::value_ptr(camera->GetEyePos()));
		defaultShader.Unuse();
		planeTransformation.Rotate(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		planeTransformation.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
		defaultShader.Use();
			glUniformMatrix4fv(UniformLoc::MODEL, 1, false, glm::value_ptr(planeTransformation.GetModel()));
			glUniformMatrix4fv(UniformLoc::INVERSE_TRANSPOSE, 1, false, glm::value_ptr(planeTransformation.GetInverseTranspose()));
			planeMaterial.Use(defaultShader.GetProgram());
			checkeredTex.Use(defaultShader.GetProgram(), "maps.diffuse", 0);
			planeMesh.DrawElements();
			checkeredTex.Unuse();
		defaultShader.Unuse();
		glDisable(GL_BLEND);

		// Center sphere
		defaultShader.Use();			
			glUniform3fv(UniformLoc::EYE_POSITION, 1, glm::value_ptr(camera->GetEyePos()));
		defaultShader.Use();				
		loadedMeshTransformation.Scale(glm::vec3(50.0f));
		loadedMeshTransformation.Translate(glm::vec3(0.0f, 5.0f, 0.0f));
		defaultShader.Use();
			glUniformMatrix4fv(UniformLoc::MODEL, 1, false, glm::value_ptr(loadedMeshTransformation.GetModel()));
			glUniformMatrix4fv(UniformLoc::INVERSE_TRANSPOSE, 1, false, glm::value_ptr(loadedMeshTransformation.GetInverseTranspose()));						
			loadedMeshMaterial.Use(defaultShader.GetProgram());
			marbleTex.Use(defaultShader.GetProgram(), "maps.diffuse", 0);
			loadedMesh.DrawElements();
			marbleTex.Unuse();
		defaultShader.Unuse();	

		// Reflective/Refractive spheres
		reflRefrShader.Use();			
			glUniform3fv(UniformLoc::EYE_POSITION, 1, glm::value_ptr(camera->GetEyePos()));
		reflRefrShader.Use();
		for (GLfloat i = -10.0f; i <= 10.0f; i += 20.0f)
		{			
			loadedMeshTransformation.Scale(glm::vec3(50.0f));
			loadedMeshTransformation.Translate(glm::vec3(i, 5.0f, 0.0f));
			reflRefrShader.Use();
				glUniformMatrix4fv(UniformLoc::MODEL, 1, false, glm::value_ptr(loadedMeshTransformation.GetModel()));
				glUniformMatrix4fv(UniformLoc::INVERSE_TRANSPOSE, 1, false, glm::value_ptr(loadedMeshTransformation.GetInverseTranspose()));
				skyboxTex.Use();
				loadedMeshMaterial.Use(reflRefrShader.GetProgram());
				loadedMesh.DrawElements();
				skyboxTex.Unuse();
			reflRefrShader.Unuse();
		}

		// Lights
		ActivateDirectionalLights(defaultShaderNM);

		// Wall
		defaultShaderNM.Use();
			glUniform3fv(UniformLoc::EYE_POSITION, 1, glm::value_ptr(camera->GetEyePos()));
		defaultShaderNM.Unuse();
		planeTransformation.Rotate(glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
		planeTransformation.Translate(glm::vec3(0.0f, 15.0f, 15.0f));		
		defaultShaderNM.Use();
			glUniformMatrix4fv(UniformLoc::MODEL, 1, false, glm::value_ptr(planeTransformation.GetModel()));
			glUniformMatrix4fv(UniformLoc::INVERSE_TRANSPOSE, 1, false, glm::value_ptr(planeTransformation.GetInverseTranspose()));			
			wallMaterial.Use(defaultShaderNM.GetProgram());
			wallDiffuseTex.Use(defaultShaderNM.GetProgram(), "maps.diffuse", 0);
			wallNormalTex.Use(defaultShaderNM.GetProgram(), "maps.normal", 1);
			planeMesh.DrawElements();
			wallNormalTex.Unuse();
			wallDiffuseTex.Unuse();
		defaultShaderNM.Unuse();	

		// Skybox		
		skyboxTransformation.Scale(glm::vec3(500.0f));
		skyboxTransformation.Translate(camera->GetEyePos());
		skyboxShader.Use();
			glUniformMatrix4fv(UniformLoc::MODEL, 1, false, glm::value_ptr(skyboxTransformation.GetModel()));
			glFrontFace(GL_CW);
			skyboxTex.Use();
			cubeMesh.DrawArrays();
			skyboxTex.Unuse();
			glFrontFace(GL_CCW);
		skyboxShader.Unuse();
	}

	void RenderReflection()
	{
		// Lights
		LightsInvertY();
		AcivateLights(defaultShader);
		LightsInvertY();

		// Center sphere
		defaultShader.Use();
			glUniform3fv(UniformLoc::EYE_POSITION, 1, glm::value_ptr(camera->GetEyePos()));
		defaultShader.Use();
		loadedMeshTransformation.Scale(glm::vec3(50.0f));
		loadedMeshTransformation.Translate(glm::vec3(0.0f, -5.0f, 0.0f));
		defaultShader.Use();
			glUniformMatrix4fv(UniformLoc::MODEL, 1, false, glm::value_ptr(loadedMeshTransformation.GetModel()));
			glUniformMatrix4fv(UniformLoc::INVERSE_TRANSPOSE, 1, false, glm::value_ptr(loadedMeshTransformation.GetInverseTranspose()));
			loadedMeshMaterial.Use(defaultShader.GetProgram());
			marbleTex.Use(defaultShader.GetProgram(), "maps.diffuse", 0);
			loadedMesh.DrawElements();
			marbleTex.Unuse();
		defaultShader.Unuse();
	}

	~Renderer() { }
};

#endif