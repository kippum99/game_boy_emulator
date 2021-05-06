#pragma once

#include "Memory.h"


class Timer {
public:
	Timer(Memory& memory);

	void update(unsigned int cycles);
private:
	// DIV register gets incremented at 16384 Hz (once every 256 clock cycles)
	const unsigned int CYCLES_PER_DIV_INCREMENT = 256;

	Memory& _memory;

	// Number of cycles elapsed with the current DIV value, which gets reset at CYCLES_PER_DIV_INCREMENT
	unsigned int _curr_div_cycles;
};

