#pragma once

#include "utils.h"


/* Helper functions for splitting or combining msb and lsb*/

// Returns the most significant byte from the 16-bit value
u8 msb(const u16 val) {
    return val >> 8;
}

// Returns the least significant byte from the 16-bit value
u8 lsb(const u16 val) {
    return val & 0b00001111;
}


// Returns an unsigned 16-bit value from the given lsb and msb 
u16 get_u16(const u8 lsb, const u8 msb) {
    return msb << 8 | lsb;
}


// Sets the given bit of the 8-bit value to the given bit value 
u8 set_bit(const u8 val, const u3 bit_pos, const bool bit_val) {
    u8 mask = 1 << bit_pos;

    if (bit_val) {
        return val | mask;
    }
    else {
        return val & ~mask;
    }
}

// Gets the given bit from the 8-bit value 
bool get_bit(const u8 val, const u3 bit_pos) {
    return (val >> bit_pos) & 1;
}