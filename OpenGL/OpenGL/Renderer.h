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

	static const GLuint NUM_SHADERS = 4;
	Shader defaultShader;
	Shader defaultShaderNM;
	Shader skyboxShader;
	Shader reflRefrShader;

	static const GLuint NUM_POINT_LIGHTS = 3;
	DirectionalLight directionalLight;	
	PointLight pointLights[NUM_POINT_LIGHTS];

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

	Texture shadowMapTex;
	CubemapTexture skyboxTex;

	GLuint UBO;

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
			glm::vec3(0.05f), glm::vec3(0.1f), glm::vec3(0.5f),
			glm::vec3(0.0f, 15.0f, 50.0f));
		
		float f = 0.5f;
		pointLights[0] = PointLight(
			glm::vec3(0.0f), glm::vec3(f, f, 0.0f), glm::vec3(0.0f),
			glm::vec3(10.0f, 1.0f, 0.0f),
			1.0f, 0.07f, 0.017f);
		pointLights[1] = PointLight(
			glm::vec3(0.0f), glm::vec3(f, 0.0f, f), glm::vec3(0.0f),
			glm::vec3(-10.0f, 1.0f, 0.0f),
			1.0f, 0.07f, 0.017f);
		pointLights[2] = PointLight(
			glm::vec3(0.0f), glm::vec3(0.0f, f, f), glm::vec3(0.0f),
			glm::vec3(0.0f, 1.0f, -10.0f),
			1.0f, 0.07f, 0.017f);
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
		
	}

	void AcivateLights(Shader& shader)
	{
		ActivateDirectionalLights(shader);
		ActivatePointLights(shader);
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
		loadedMeshMaterial	 = Material(glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 128);		

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
		const GLchar* UB_NAME = "ViewProjectionLighSpace";
		const GLuint PROGRAMS[NUM_SHADERS] = { defaultShader.GetProgram(), defaultShaderNM.GetProgram(), skyboxShader.GetProgram(), reflRefrShader.GetProgram() };  // add the reference to the new shader's program here
		GLuint i = 0;

		glGenBuffers(1, &UBO);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT0, UBO);
		
		for (i = 0; i < NUM_SHADERS; ++i)
		{
			UBIndices[i] = glGetUniformBlockIndex(PROGRAMS[i], UB_NAME);
			glUniformBlockBinding(PROGRAMS[i], UBIndices[i], BINDING_POINT0);
		}		
	}

	// For animation	
	GLfloat dt = 0.0f;
	
public:
	void SetDeltaTime(GLfloat deltaTime) { dt += deltaTime; }

	void SetProjectionMatrix(glm::mat4 projection)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void SetViewMatrix(glm::mat4 view)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void SetLightSpaceMatrix(glm::mat4 lightSpace) {
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(lightSpace));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void SetShadowMapTexture(const GLuint& shadowMapTex) { this->shadowMapTex = shadowMapTex; }

	glm::vec3 GetDirectionalLightPosition() { return directionalLight.GetPosition(); }
	
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
			glUniform1i(glGetUniformLocation(defaultShader.GetProgram(), "reflection"), false);
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
		planeTransformation.Rotate(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		planeTransformation.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
		defaultShader.Use();
			glUniform3fv(UniformLoc::EYE_POSITION, 1, glm::value_ptr(camera->GetEyePos()));
			glUniformMatrix4fv(UniformLoc::MODEL, 1, false, glm::value_ptr(planeTransformation.GetModel()));
			glUniformMatrix4fv(UniformLoc::INVERSE_TRANSPOSE, 1, false, glm::value_ptr(planeTransformation.GetInverseTranspose()));
			glUniform1i(glGetUniformLocation(defaultShader.GetProgram(), "reflection"), false);
			planeMaterial.Use(defaultShader.GetProgram());
			checkeredTex.Use(defaultShader.GetProgram(), "maps.diffuse", 0);
			shadowMapTex.Use(defaultShader.GetProgram(), "maps.shadow", 3);
			planeMesh.DrawElements();
			checkeredTex.Unuse();
		defaultShader.Unuse();
		glDisable(GL_BLEND);

		// Center sphere
		loadedMeshTransformation.Scale(glm::vec3(50.0f));
		loadedMeshTransformation.Translate(glm::vec3(0.0f, 5.0f, 0.0f));
		defaultShader.Use();			
			glUniform3fv(UniformLoc::EYE_POSITION, 1, glm::value_ptr(camera->GetEyePos()));
			glUniformMatrix4fv(UniformLoc::MODEL, 1, false, glm::value_ptr(loadedMeshTransformation.GetModel()));
			glUniformMatrix4fv(UniformLoc::INVERSE_TRANSPOSE, 1, false, glm::value_ptr(loadedMeshTransformation.GetInverseTranspose()));
			glUniform1i(glGetUniformLocation(defaultShader.GetProgram(), "reflection"), false);
			loadedMeshMaterial.Use(defaultShader.GetProgram());
			marbleTex.Use(defaultShader.GetProgram(), "maps.diffuse", 0);
			shadowMapTex.Use(defaultShader.GetProgram(), "maps.shadow", 3);
			loadedMesh.DrawElements();
			marbleTex.Unuse();
		defaultShader.Unuse();	

		// Lights
		AcivateLights(reflRefrShader);

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
				glUniform1i(glGetUniformLocation(reflRefrShader.GetProgram(), "reflection"), false);
				skyboxTex.Use();
				marbleTex.Use(reflRefrShader.GetProgram(), "maps.diffuse", 0);
				shadowMapTex.Use(reflRefrShader.GetProgram(), "maps.shadow", 3);
				loadedMeshMaterial.Use(reflRefrShader.GetProgram());
				loadedMesh.DrawElements();
				skyboxTex.Unuse();
			reflRefrShader.Unuse();
		}

		// Lights
		ActivateDirectionalLights(defaultShaderNM);

		// Wall
		glm::mat4 m1;
		glm::mat4 m2;		
		m1 = glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		planeTransformation.Rotate(m1);
		planeTransformation.Translate(glm::vec3(0.0f, 15.0f, -15.0f));
		RenderWall();				
		m2 = glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		planeTransformation.Rotate(m2 * m1);
		planeTransformation.Translate(glm::vec3(15.0f, 15.0f, 0.0f));	
		RenderWall();
		m2 = glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		planeTransformation.Rotate(m2 * m1);
		planeTransformation.Translate(glm::vec3(-15.0f, 15.0f, 0.0f));		
		RenderWall();

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

	void RenderWall()
	{
		defaultShaderNM.Use();
			glUniform3fv(UniformLoc::EYE_POSITION, 1, glm::value_ptr(camera->GetEyePos()));
			glUniformMatrix4fv(UniformLoc::MODEL, 1, false, glm::value_ptr(planeTransformation.GetModel()));
			glUniformMatrix4fv(UniformLoc::INVERSE_TRANSPOSE, 1, false, glm::value_ptr(planeTransformation.GetInverseTranspose()));	
			glUniform1i(glGetUniformLocation(defaultShaderNM.GetProgram(), "reflection"), false);
			wallMaterial.Use(defaultShaderNM.GetProgram());
			wallDiffuseTex.Use(defaultShaderNM.GetProgram(), "maps.diffuse", 0);
			wallNormalTex.Use(defaultShaderNM.GetProgram(), "maps.normal", 1);
			shadowMapTex.Use(defaultShaderNM.GetProgram(), "maps.shadow", 3);
			planeMesh.DrawElements();
			wallNormalTex.Unuse();
			wallDiffuseTex.Unuse();
		defaultShaderNM.Unuse();
	}

	void RenderReflection()
	{
		// Lights
		LightsInvertY();
		AcivateLights(defaultShader);
		LightsInvertY();

		// Center sphere
		loadedMeshTransformation.Scale(glm::vec3(50.0f));
		loadedMeshTransformation.Translate(glm::vec3(0.0f, -5.0f, 0.0f));
		defaultShader.Use();
			glUniform3fv(UniformLoc::EYE_POSITION, 1, glm::value_ptr(camera->GetEyePos()));
			glUniformMatrix4fv(UniformLoc::MODEL, 1, false, glm::value_ptr(loadedMeshTransformation.GetModel()));
			glUniformMatrix4fv(UniformLoc::INVERSE_TRANSPOSE, 1, false, glm::value_ptr(loadedMeshTransformation.GetInverseTranspose()));
			glUniform1i(glGetUniformLocation(defaultShader.GetProgram(), "reflection"), true);
			loadedMeshMaterial.Use(defaultShader.GetProgram());
			marbleTex.Use(defaultShader.GetProgram(), "maps.diffuse", 0);
			loadedMesh.DrawElements();
			marbleTex.Unuse();
		defaultShader.Unuse();

		// Lights
		LightsInvertY();
		AcivateLights(reflRefrShader);
		LightsInvertY();

		// Reflective/Refractive spheres
		reflRefrShader.Use();			
			glUniform3fv(UniformLoc::EYE_POSITION, 1, glm::value_ptr(camera->GetEyePos()));
		reflRefrShader.Use();
		for (GLfloat i = -10.0f; i <= 10.0f; i += 20.0f)
		{			
			loadedMeshTransformation.Scale(glm::vec3(50.0f));
			loadedMeshTransformation.Translate(glm::vec3(i, -5.0f, 0.0f));
			reflRefrShader.Use();
				glUniformMatrix4fv(UniformLoc::MODEL, 1, false, glm::value_ptr(loadedMeshTransformation.GetModel()));
				glUniformMatrix4fv(UniformLoc::INVERSE_TRANSPOSE, 1, false, glm::value_ptr(loadedMeshTransformation.GetInverseTranspose()));
				glUniform1i(glGetUniformLocation(reflRefrShader.GetProgram(), "reflection"), true);
				skyboxTex.Use();				
				loadedMeshMaterial.Use(reflRefrShader.GetProgram());
				loadedMesh.DrawElements();
				skyboxTex.Unuse();
			reflRefrShader.Unuse();
		}
	}

	~Renderer() { }
};

#endif