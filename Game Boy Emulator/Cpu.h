#pragma once

#include "Memory.h"
#include "Registers.h"
#include "types.h"


class Cpu {
public:
	bool is_on;

	// TODO: Move to private, but it's public for debugging for now.
	Registers _registers;

	Cpu(Memory& memory);

	unsigned int execute_next();
	void handle_interrupt();
	void service_interrupt(u3 interrupt_bit);
private:
	Memory& _memory;
	//Registers _registers;
	
	// Interrupt master enable flag
	bool _ime;

	/* For reading ROM file directly - must replace later */

	// Reads the next 8 bits from memory at pc address and increments pc
	u8 _read8();

	// Reads the next 16 bits from memroy at pc address and increments pc twice.
	u16 _read16();


	/* Instrunction helper functions */

	void _push_stack(const u16 val);
	u16 _pop_stack();

	void _adc_a(const u8 n);
	u8 _add(const u8 val1, const u8 val2);
	void _add_hl(const u16 n);
	u8 _and(const u8 val1, const u8 val2);
	void _cp(const u8 val1, const u8 val2);
	u8 _inc(const u8 val);
	u8 _or(const u8 val1, const u8 val2);
	u8 _rlc(const u8 val);
	u8 _rl(const u8 val);
	u8 _rr(const u8 val);
	void _rst(const u8 n);
	u8 _sla(const u8 val);
	u8 _sra(const u8 val);
	u8 _srl(const u8 val);
	u8 _sub(const u8 val1, const u8 val2);
	u8 _swap(const u8 n);
};

