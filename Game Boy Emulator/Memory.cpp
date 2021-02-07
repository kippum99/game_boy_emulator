#include "Memory.h"

Memory::Memory() {
	_memory = new u8[65536];
}

u8 Memory::read(const u16 addr) const {
	return _memory[addr];
}

void Memory::write(const u16 addr, const u8 val) {
	_memory[addr] = val;
}