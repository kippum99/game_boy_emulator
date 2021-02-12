#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include "Cpu.h"
#include "Memory.h"
#include "Registers.h"


using namespace std;


int main() {
    ifstream rom_file("roms/01-special.gb", ios::in | ios::binary | ios::ate);

    assert(rom_file.is_open());

    char* rom;
    size_t rom_size = rom_file.tellg();
    
    // TODO: Remove after implementing rom bank mapping
    assert(rom_size == 32768);

    rom = new char[rom_size];
    rom_file.seekg(0, ios::beg);
    rom_file.read(rom, rom_size);
    rom_file.close();

    Memory memory{};
    Cpu cpu{ memory };

    memory.load_rom((u8*)rom);
    cpu.execute();

    return 0;
}