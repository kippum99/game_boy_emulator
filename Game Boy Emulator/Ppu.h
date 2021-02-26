#pragma once

#include <SDL.h>

#include "Memory.h"


class Ppu {
public:
	Ppu(Memory& memory);

	void render();

private:
	Memory& _memory;

	//SDL_Event _event;
	SDL_Window* _window;
	SDL_Renderer* _renderer;
};

