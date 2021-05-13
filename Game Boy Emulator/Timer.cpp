#include "Timer.h"
#include "utils.h"


Timer::Timer(Memory& memory) : _memory(memory) {
	_curr_div_cycles = 0;
	_curr_tima_cycles = 0;
}

void Timer::update(const unsigned int cycles) {
	_update_divider(cycles);

	if (_is_timer_enabled()) {
		_update_timer(cycles);
	}
}

// Increments DIV register value
void Timer::_update_divider(const unsigned int cycles) {
	_curr_div_cycles += cycles;

	if (_curr_div_cycles >= CYCLES_PER_DIV_INCREMENT) {
		_curr_div_cycles %= CYCLES_PER_DIV_INCREMENT;
		_memory.increment_div_register();
	}
}

// Increments TIMA register value
void Timer::_update_timer(const unsigned int cycles) {
	// Increment TIMA register value
	_curr_tima_cycles += cycles;

	const unsigned int cycles_per_tima_increment = _get_cycles_per_tima_increment();

	if (_curr_tima_cycles >= cycles_per_tima_increment) {
		_curr_tima_cycles %= cycles_per_tima_increment;

		// Increment TIMA (FF05)
		const u8 tima = _memory.read(0xFF05);

		if (tima == 0xFF) {
			// If TIMA overflowed, reset it to TMA and request a timer interrupt.
			_memory.write(0xFF05, _memory.read(0xFF06));
			_memory.request_interrupt(2);
		}
		else {
			_memory.write(0xFF05, tima + 1);
		}
	}
}

// Returns true if timer is enabled (specified in the TAC register),
// false otherwise.
bool Timer::_is_timer_enabled() {
	const u8 tac = _memory.read(0xFF07);

	return get_bit(tac, 2);
}

// Returns the number of cycles that must elapse before TIMA increment
// based on the frequency specified in the TAC register.
unsigned int Timer::_get_cycles_per_tima_increment() {
	const u8 tac = _memory.read(0xFF07);

	switch (tac & 0b11) {
	case 0:
		return 1024;	// 4096 Hz
	case 1:
		return 16;		// 262144 Hz
	case 2:
		return 64;		// 65536 Hz
	case 3:
		return 256;		// 16382 Hz
	}
}
