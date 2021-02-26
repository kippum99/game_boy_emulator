#include <iostream>

#include "Memory.h"

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
		exit(1);
	}
	else if (addr >= 0xE000 and addr <= 0xFDFF) {
		printf("ERROR: Use of prohibited address %X\n", addr);
		exit(1);
	}
	else if (addr >= 0xFEA0 and addr <= 0xFEFF) {
		printf("ERROR: Use of prohibited address %X\n", addr);
		exit(1);
	}
	
	_memory[addr] = val;
}