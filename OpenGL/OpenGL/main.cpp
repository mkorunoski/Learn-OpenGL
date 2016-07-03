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

GLuint wndWidth  = 1440;
GLuint wndHeight = 900;

int main(int argc, char ** argv)
{	
	Display display(wndWidth, wndHeight);
	Camera camera(glm::vec3(0.0f, 5.0f, 20.0f));

	EventHandler eventHandler;

	Timer timer; timer.Start();

	Renderer renderer(&camera, wndWidth, wndHeight);
	
	SDL_SetRelativeMouseMode(SDL_TRUE);	
	SDL_Event e;		
	while (true)
	{
		display.Clear(0.0f, 0.0f, 0.0f, 1.0f);

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

				case SDLK_z: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
				case SDLK_x: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;

				case SDLK_c: renderer.ChangeOutlineCube(); break;

				case SDLK_1: renderer.whichLight = 1; break;
				case SDLK_2: renderer.whichLight = 2; break;
				case SDLK_3: renderer.whichLight = 3; break;
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
		
		renderer.IncrementAngle((GLfloat)timer.DeltaTime());
		renderer.RenderScene();

		display.SwapBuffers();
	}

	return 0;
}