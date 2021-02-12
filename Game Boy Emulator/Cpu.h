#pragma once

#include "Memory.h"
#include "Registers.h"
#include "types.h"


class Cpu {
public:
	Cpu(Memory& memory);

	void execute();
	
private:
	Memory& _memory;
	Registers _registers;
	

	/* For reading ROM file directly - must replace later */

	// Reads the next 8 bits from memory at pc address and increments pc
	u8 _read8();

	// Reads the next 16 bits from memroy at pc address and increments pc twice.
	u16 _read16();


	/* Instrunction helper functions */

	void _push_stack(const u16 val);
	u16 _pop_stack();

	u8 _and(const u8 val1, const u8 val2);
	u8 _or(const u8 val1, const u8 val2);
	void _cp(const u8 val1, const u8 val2);
	u8 _inc(const u8 val);
	u8 _rr(const u8 val);
	u8 _sra(const u8 val);
	u8 _srl(const u8 val);
};

