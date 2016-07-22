#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "Shader.h"

class Display
{
private:
	SDL_Window* window;
	SDL_GLContext glContext;
	GLuint width;
	GLuint height;

public:
	Display(GLuint width, GLuint height)
	{
		this->width = width;
		this->height = height;

		SDL_Init(SDL_INIT_EVERYTHING);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		SDL_SetRelativeMouseMode(SDL_TRUE);

		window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
		glContext = SDL_GL_CreateContext(window);
		
		glewInit();

		glViewport(0, 0, width, height);
		glEnable(GL_DEPTH_TEST);
		/*glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);*/
		glEnable(GL_FRAMEBUFFER_SRGB);

		InitOffscreenRenderTarget();
		InitDepthMapRenderTarget();
	}

	void Clear(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
	{
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void SwapBuffers()
	{
		SDL_GL_SwapWindow(window);
	}
	
	~Display()
	{
		SDL_GL_DeleteContext(glContext);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

private:
	GLuint framebuffer;
	GLuint framebufferColorTex;
	GLuint framebufferDepthTex;

public:
	void InitOffscreenRenderTarget()
	{		
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
					
		glGenTextures(1, &framebufferColorTex);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferColorTex);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_SRGB, width, height, GL_TRUE);		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferColorTex, 0);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		glGenTextures(1, &framebufferDepthTex);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDepthTex);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH_COMPONENT24, width, height, GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, framebufferDepthTex, 0);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);				
	}

	void RenderSceneToFrameBuffer()
	{	
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);	
		Clear(0.0f, 0.0f, 0.0f, 1.0f);
	}	

	void DisplayFrameBufferContent()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

private:
		GLuint depthMapFramebuffer;
		GLuint depthMapFramebufferDepthTex;
		GLuint VAO;
		Shader offscreenShader;

public:
	const GLuint& GetShadowMapTexure() { return depthMapFramebufferDepthTex; }

	void InitDepthMapRenderTarget()
	{
		glGenFramebuffers(1, &depthMapFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebuffer);
		
		glGenTextures(1, &depthMapFramebufferDepthTex);
		glBindTexture(GL_TEXTURE_2D, depthMapFramebufferDepthTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapFramebufferDepthTex, 0);		
		glBindTexture(GL_TEXTURE_2D, 0);
				
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		offscreenShader = Shader("./res/shaders/offscreen.vs", "./res/shaders/offscreen.fs", "offscreen");

		GLfloat quad[] = {
			-1.0f, -1.0f,  0.0f, 0.0f,
			+1.0f, -1.0f,  1.0f, 0.0f,
			+1.0f, +1.0f,  1.0f, 1.0f,

			-1.0f, -1.0f,  0.0f, 0.0f,
			+1.0f, +1.0f,  1.0f, 1.0f,
			-1.0f, +1.0f,  0.0f, 1.0f
		};

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		GLuint VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad[0], GL_STATIC_DRAW);		

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (const GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (const GLvoid*)(2 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
	}
	
	void RenderSceneToDepthMap()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebuffer);
		Clear(0.0f, 0.0f, 0.0f, 1.0f);
	}

	void DisplayDepthMapContent()
	{
		offscreenShader.Use();
			glUniform1i(glGetUniformLocation(offscreenShader.GetProgram(), "screenTexture"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, depthMapFramebufferDepthTex);
			glBindVertexArray(VAO);				
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
		offscreenShader.Unuse();
	}

	void RenderSceneOnscreen()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};

#endif