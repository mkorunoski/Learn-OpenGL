#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL2/SDL.h>
#include <SOIL/SOIL.h>

#include "Display.h"
#include "Camera.h"
#include "EventHandler.h"
#include "Timer.h"
#include "Renderer.h"

GLuint wndWidth  = 1024;
GLuint wndHeight = 768;

int main(int argc, char ** argv)
{	
	Display display(wndWidth, wndHeight);
	Camera camera(glm::vec3(0.0f, 5.0f, 20.0f));

	EventHandler eventHandler;

	Timer timer; timer.Start();

	Renderer renderer(&camera, wndWidth, wndHeight);

	float w = 25.0f;
	glm::mat4 projectionOrtho = glm::ortho(-w, w, -w, w, 0.1f, 1000.0f);
	glm::mat4 projectionPersp = glm::perspective(70.0f, (GLfloat)wndWidth / (GLfloat)wndHeight, 0.1f, 1000.0f);
	glm::mat4 view		 = glm::lookAt(renderer.GetDirectionalLightPosition(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpace = projectionOrtho * view;
	
	SDL_Event e;		
	while (true)
	{
		display.Clear(0.2f, 0.2f, 0.2f, 1.0f);

		timer.Tick();

		int mouseXpos = 0, mouseYpos = 0;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_a: eventHandler.KeyPressed(KEY_LEFT);  break;
				case SDLK_d: eventHandler.KeyPressed(KEY_RIGHT); break;
				case SDLK_w: eventHandler.KeyPressed(KEY_UP);	 break;
				case SDLK_s: eventHandler.KeyPressed(KEY_DOWN);	 break;
				}
				switch (e.key.keysym.sym)
				{
				case SDLK_ESCAPE: return 0;				
				}
			}
			if (e.type == SDL_KEYUP)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_a: eventHandler.KeyReleased(KEY_LEFT);  break;
				case SDLK_d: eventHandler.KeyReleased(KEY_RIGHT); break;
				case SDLK_w: eventHandler.KeyReleased(KEY_UP);	  break;
				case SDLK_s: eventHandler.KeyReleased(KEY_DOWN);  break;
				}
			}			
			if (e.type == SDL_MOUSEMOTION)
			{
				mouseXpos = e.motion.xrel;
				mouseYpos = e.motion.yrel;
			}			
		}
		eventHandler.Process(&camera, (GLfloat)timer.DeltaTime(), (GLfloat)mouseXpos, (GLfloat)mouseYpos);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		renderer.SetDeltaTime((GLfloat)timer.DeltaTime());	

		
		renderer.SetProjectionMatrix(projectionOrtho);
		renderer.SetViewMatrix(view);
		display.RenderSceneToDepthMap();
		renderer.RenderScene();
		display.RenderSceneOnscreen();
		//display.DisplayDepthMapContent();		
		
		renderer.SetProjectionMatrix(projectionPersp);
		renderer.SetViewMatrix(camera.GetViewMatrix());
		renderer.SetLightSpaceMatrix(lightSpace);
		renderer.SetShadowMapTexture(display.GetShadowMapTexure());
		display.RenderSceneToFrameBuffer();
		renderer.RenderScene();
		display.DisplayFrameBufferContent();

		display.SwapBuffers();
	}

	return 0;
}