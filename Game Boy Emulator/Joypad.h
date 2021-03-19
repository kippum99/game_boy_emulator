#pragma once

#include "Memory.h"


class Joypad {
public:
	Joypad(Memory& memory);

	void update();
private:
	Memory& _memory;
};

