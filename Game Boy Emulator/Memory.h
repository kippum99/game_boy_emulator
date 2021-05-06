#pragma once

#include "types.h"


class Memory {
public:
	Memory();

	void load_rom(const u8* rom);

	u8 read(const u16 addr) const;
	void write(const u16 addr, const u8 val);

	void request_interrupt(u3 interrupt_bit);

	void increment_div_register();
	void reset_div_register();

private:
	u8* _memory;

	void _do_dma_transfer(const u16 source_start_addr);
};