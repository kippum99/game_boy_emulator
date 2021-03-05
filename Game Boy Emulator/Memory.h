#pragma once

#include "types.h"


class Memory {
public:
	Memory();

	void load_rom(const u8* rom);

	u8 read(const u16 addr) const;
	void write(const u16 addr, const u8 val);

	void request_interrupt(u3 interrupt_bit);

private:
	u8* _memory;
};