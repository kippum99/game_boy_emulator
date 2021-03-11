#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include "GameBoy.h"


using namespace std;


int main(int argc, char* args[]) {
    u8* cartridge_memory = new u8[0x200000];

    FILE* rom_file;
    rom_file = fopen("roms/01-special.gb", "rb");
    //rom_file = fopen("roms/dmg-acid2.gb", "rb");
    //rom_file = fopen("roms/tetris.gb", "rb");

    fread(cartridge_memory, 1, 0x200000, rom_file);
    fclose(rom_file);

    GameBoy gameboy{};
    gameboy.load_rom(cartridge_memory);
    
    while (true) {
        gameboy.update_frame();
    }

    return 0;
}