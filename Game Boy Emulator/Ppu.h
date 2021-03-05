#pragma once

#include <SDL.h>

#include "Memory.h"


class Ppu {
public:
	Ppu(Memory& memory);

	void update(const unsigned int cycles);
	void render();

private:
	const unsigned int CYCLES_PER_SCANLINE = 456;

	Memory& _memory;

	// Number of cycles elapsed in the current scan line, which gets reset at CYCLES_PER_SCANLINE
	unsigned int _curr_scanline_cycles;

	// 160 x 144 pixel matrix
	u3* _pixel_matrix;

	//SDL_Event _event;
	SDL_Window* _window;
	SDL_Renderer* _renderer;

	void _set_lcd_status();
	void _draw_scanline(const unsigned int scanline);
};

