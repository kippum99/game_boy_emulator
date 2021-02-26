#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include "GameBoy.h"


using namespace std;


int main(int argc, char* args[]) {
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

    GameBoy gameboy{};
    gameboy.load_rom((u8*)rom);
    
    while (true) {
        gameboy.update_frame();
    }

    return 0;
}