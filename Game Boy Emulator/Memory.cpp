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
		//printf("ERROR: Cannot write to ROM at address %X\n", addr);
	}
	else if (addr >= 0xE000 and addr <= 0xFDFF) {
		printf("ERROR: Use of prohibited address %X\n", addr);
	}
	else if (addr >= 0xFEA0 and addr <= 0xFEFF) {
		//printf("ERROR: Use of prohibited address %X\n", addr);
	}
	else if (addr == 0xFF04) {
		// Writing to the DIV register resets it to $00
		_memory[addr] = 0;
	}
	else if (addr == 0xFF46) {
		// Initiate DMA Transfer
		_do_dma_transfer(val << 8);

		_memory[addr] = val;
	}
	else {
		_memory[addr] = val;
	}
}

void Memory::request_interrupt(u3 interrupt_bit) {
	u8 interrupt_flag = _memory[0xFF0F];
	_memory[0xFF0F] = set_bit(interrupt_flag, interrupt_bit, 1);
}

// Increments the divider register (FF04 - DIV).
void Memory::increment_div_register() {
	_memory[0xFF04]++;
}

// Resets the divider register (FF04 - DIV).
void Memory::reset_div_register() {
	_memory[0xFF04] = 0;
}


void Memory::_do_dma_transfer(const u16 source_start_addr) {
	// Total of 0xA0 bytes will be copied from source to 
	// destination area (0xFE00-0xFE9F)
	for (int i = 0; i <= 0x9F; i++) {
		_memory[0xFE00 + i] = _memory[source_start_addr + i];
	}
}