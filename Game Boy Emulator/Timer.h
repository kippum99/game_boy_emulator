#pragma once

#include "Memory.h"


class Timer {
public:
	Timer(Memory& memory);

	void update(const unsigned int cycles);
private:
	// DIV register gets incremented at 16384 Hz (once every 256 clock cycles)
	const unsigned int CYCLES_PER_DIV_INCREMENT = 256;

	Memory& _memory;

	// Number of cycles elapsed with the current DIV value, which gets reset at CYCLES_PER_DIV_INCREMENT
	unsigned int _curr_div_cycles;

	// Number of cycles elapsed with the current TIMA value, which gets reset when TIMA gets incremented
	// at the frequency specified in TAC
	unsigned int _curr_tima_cycles;

	void _update_divider(const unsigned int cycles);
	void _update_timer(const unsigned int cycles);

	bool _is_timer_enabled();
	unsigned int _get_cycles_per_tima_increment();
};

