#pragma once


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