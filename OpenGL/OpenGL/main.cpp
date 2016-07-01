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

GLuint wndWidth  = 1366;
GLuint wndHeight = 768;

int numLights = 1;

int main(int argc, char ** argv)
{	
	Display display(wndWidth, wndHeight);
	Camera camera(glm::vec3(0.0f, 5.0f, 20.0f));

	EventHandler eventHandler;

	Timer timer;
	timer.Start();

	Renderer renderer(wndWidth, wndHeight);
	
	SDL_SetRelativeMouseMode(SDL_TRUE);	
	SDL_Event e;		
	while (true)
	{
		display.Clear(0.0f, 0.0f, 0.0f, 1.0f);

		timer.Tick();
		
		SDL_PollEvent(&e);		
		if (e.type == SDL_KEYDOWN)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_LEFT:		eventHandler.KeyPressed(KEY_LEFT);  break;
			case SDLK_RIGHT:	eventHandler.KeyPressed(KEY_RIGHT); break;
			case SDLK_UP:		eventHandler.KeyPressed(KEY_UP);	break;
			case SDLK_DOWN:		eventHandler.KeyPressed(KEY_DOWN);	break;

			case SDLK_ESCAPE: return 0;

			case SDLK_q: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
			case SDLK_w: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;							

			case SDLK_1: numLights = 1; break;
			case SDLK_2: numLights = 2; break;
			case SDLK_3: numLights = 3; break;
			}					
		}
		if (e.type == SDL_KEYUP)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_LEFT:		eventHandler.KeyReleased(KEY_LEFT);  break;
			case SDLK_RIGHT:	eventHandler.KeyReleased(KEY_RIGHT); break;
			case SDLK_UP:		eventHandler.KeyReleased(KEY_UP);	 break;
			case SDLK_DOWN:		eventHandler.KeyReleased(KEY_DOWN);  break;
			}					
		}		
		int xpos = 0, ypos = 0;
		if (e.type == SDL_MOUSEMOTION)
		{	
			xpos = e.motion.xrel;
			ypos = e.motion.yrel;						
		}		
		e.type = NULL;

		eventHandler.Process(&camera, (GLfloat)timer.DeltaTime(), (GLfloat)xpos, (GLfloat)ypos);		
		
		renderer.RenderScene(camera, numLights, (GLfloat)timer.DeltaTime());

		display.SwapBuffers();
	}

	return 0;
}