#pragma once
#include <SDL.h>

class Controls
{
public:
	SDL_Event event;

	int mouseX = 0, mouseY = 0, pastMouseX = 0, pastMouseY = 0;
	int dx, dy;
	bool mouseDown = false;

	Uint32 type;
	Uint8 button;
	SDL_Keycode key;
	
	Controls(): event()
	{
	}

	void updateState(SDL_Event& event)
	{
		pastMouseX = mouseX;
		pastMouseY = mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);

		dx = mouseX - pastMouseX;
		dy = mouseY - pastMouseY;
		
		type = event.type;
		button = event.button.button;
		key = event.key.keysym.sym;
	}
};
