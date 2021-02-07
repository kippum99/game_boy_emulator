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
    ifstream rom_file("roms/dmg-acid2.gb", ios::in | ios::binary | ios::ate);

    assert(rom_file.is_open());

    char* rom;
    size_t rom_size = rom_file.tellg();
    rom = new char[rom_size];
    rom_file.seekg(0, ios::beg);
    rom_file.read(rom, rom_size);
    rom_file.close();

    Memory memory{};
    Cpu cpu{ memory };

    cpu.run_rom((u8*)rom);


    return 0;
}