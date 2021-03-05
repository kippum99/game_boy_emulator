#include <iostream>

#include "Memory.h"
#include "utils.h"


Memory::Memory() {
	_memory = new u8[65536]{ 0 };
}


// Currently just copies 32 kb of rom into the first 32 kb of memory
void Memory::load_rom(const u8* rom) {
	for (int i = 0; i < 32768; i++) {
		_memory[i] = rom[i];
	}
}


u8 Memory::read(const u16 addr) const {
	return _memory[addr];
}


void Memory::write(const u16 addr, const u8 val) {
	if (addr <= 0x7FFF) {
		printf("ERROR: Cannot write to ROM at address %X\n", addr);
	}
	else if (addr >= 0xE000 and addr <= 0xFDFF) {
		printf("ERROR: Use of prohibited address %X\n", addr);
	}
	else if (addr >= 0xFEA0 and addr <= 0xFEFF) {
		printf("ERROR: Use of prohibited address %X\n", addr);
	}
	else {
		_memory[addr] = val;
	}
}


void Memory::request_interrupt(u3 interrupt_bit) {
	u8 interrupt_flag = _memory[0xFF0F];
	_memory[0xFF0F] = set_bit(interrupt_flag, interrupt_bit, 1);
}