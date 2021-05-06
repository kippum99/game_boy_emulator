#include "Timer.h"


Timer::Timer(Memory& memory) : _memory(memory) {
	_curr_div_cycles = 0;
}

void Timer::update(unsigned int cycles) {
	// Increment DIV register value
	_curr_div_cycles += cycles;

	if (_curr_div_cycles >= CYCLES_PER_DIV_INCREMENT) {
		_curr_div_cycles %= CYCLES_PER_DIV_INCREMENT;
		_memory.increment_div_register();
	}
}