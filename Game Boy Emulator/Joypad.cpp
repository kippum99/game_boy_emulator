#include "Joypad.h"


Joypad::Joypad(Memory& memory) : _memory(memory) {}


void Joypad::update() {
	// Update joypad input register (FF00) based on input
	u8 reg_val = _memory.read(0xFF00);

	// TODO: Actually take input - right now, no button can be pressed
	reg_val |= 0xF;

	_memory.write(0xFF00, reg_val);
}