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

		window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
		glContext = SDL_GL_CreateContext(window);
		
		glewInit();

		glViewport(0, 0, width, height);
		glEnable(GL_DEPTH_TEST);
		/*glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);*/

		InitOffscreenRenderTarget();
	}

	void Clear(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
	{
		glClearColor(r, g, b , a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void SwapBuffers()
	{
		SDL_GL_SwapWindow(window);
	}
	
	~Display()
	{
		delete(offscreenShader);
		SDL_GL_DeleteContext(glContext);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

private:
	GLuint framebuffer;
	GLuint framebufferColorTex;
	GLuint frameBufferDepthStencilRBO;

	Shader* offscreenShader;
	GLuint VAO;

public:
	void InitOffscreenRenderTarget()
	{		
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		
		glGenTextures(1, &framebufferColorTex);
		glBindTexture(GL_TEXTURE_2D, framebufferColorTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferColorTex, 0);

		glGenRenderbuffers(1, &frameBufferDepthStencilRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, frameBufferDepthStencilRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, frameBufferDepthStencilRBO);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		offscreenShader = new Shader("./res/shaders/offscreen.vs", "./res/shaders/offscreen.fs", "offscreen");
		
		GLfloat quad[] = 
		{
			-1.0f, -1.0f,  0.0f, 0.0f,
			+1.0f, -1.0f,  1.0f, 0.0f,
			+1.0f, +1.0f,  1.0f, 1.0f,

			-1.0f, -1.0f,  0.0f, 0.0f,
			+1.0f, +1.0f,  1.0f, 1.0f,
			-1.0f, +1.0f,  0.0f, 1.0f,
		};

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		GLuint VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}

	void RenderSceneToFrameBuffer()
	{	
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void DisplayFrameBufferContent()
	{	
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(VAO);
		
		offscreenShader->Use();				
		glUniform1i(glGetUniformLocation(offscreenShader->GetProgram(), "screenTexture"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, framebufferColorTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);			
		offscreenShader->Unuse();

		glBindVertexArray(0);

		glEnable(GL_DEPTH_TEST);
	}

	void RenderOnscreen()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};

#endif