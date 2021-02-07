#pragma once

#include "Memory.h"
#include "Registers.h"
#include "types.h"


class Cpu {
public:
	Cpu(Memory& memory);

	void run_rom(u8* rom);
	
private:
	Memory& _memory;
	Registers _registers;
	

	/* For reading ROM file directly - must replace later */

	// Reads 8 bits from ROM file (using pc)
	u8 _read8(const u8* rom);

	// Reads 16 bits from ROM file (using pc)
	u16 _read16(const u8* rom);


	/* Instrunction helper functions */
	void _cp(const u8 val1, const u8 val2);
};

