#pragma once

#include <SDL.h>

#include "Cpu.h"
#include "Joypad.h"
#include "Memory.h"
#include "Ppu.h"
#include "Timer.h"
#include "types.h"


class GameBoy {
public:
	GameBoy();

	void load_rom(const u8* rom);
	void update_frame(SDL_Renderer* renderer);

private:
	Memory _memory;
	Cpu _cpu;
	Ppu _ppu;
	Joypad _joypad;
	Timer _timer;
};

