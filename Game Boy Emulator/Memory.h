#pragma once

#include "types.h"


class Memory {
public:
	Memory();
	u8 read(const u16 addr) const;
	void write(const u16 addr, const u8 val);

private:
	u8* _memory;
};