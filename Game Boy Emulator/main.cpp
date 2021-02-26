#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include <SDL.h>

#include "Cpu.h"
#include "Ppu.h"
#include "Memory.h"
#include "Registers.h"


using namespace std;


int main(int argc, char* args[]) {

    ////SDL_Init(SDL_INIT_EVERYTHING);
    //SDL_Event event;
    //SDL_Window* window;
    //SDL_Renderer* renderer;
    //SDL_CreateWindowAndRenderer(160, 144, 0, &window, &renderer);
    //  
    //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    //SDL_RenderClear(renderer);
    //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    //SDL_RenderDrawPoint(renderer, 150, 150); //Renders on middle of screen.
    //SDL_RenderPresent(renderer);

    //while (1) {
    //    if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
    //        break;
    //}
    //SDL_DestroyRenderer(renderer);
    //SDL_DestroyWindow(window);
    //SDL_Quit();


    ifstream rom_file("roms/02-interrupts.gb", ios::in | ios::binary | ios::ate);

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
    Ppu ppu{ memory };

    memory.load_rom((u8*)rom);

    while (true) {
        for (int i = 0; i < 100; i++) {

            //if (cpu._registers.get_pc() == 0xC33F) {
            //    cout << "Calling C05A" << endl;
            //}

            if (cpu._registers.get_pc() == 0xC33D && cpu._registers.get_de() == 0x9A00) {
                cout << "check values here" << endl;
            }

            //if (cpu._registers.get_pc() == 0xC34A && cpu._registers.get_de() == 0x00F0) {
            //    cout << "check values here" << endl;
            //}

            if (cpu._registers.get_pc() == 0xC34B && cpu._registers.get_de() == 0x00F0) {
                cout << "check values here" << endl;
            }

            if (cpu._registers.get_pc() == 0xC350) {
                cout << "we want to get here" << endl;
            }

            if (cpu._registers.get_pc() == 0xC359) {
                cout << "we want to get here" << endl;
            }


            cpu.execute_next();
        }

        if (cpu._registers.get_pc() > 0x3000) {
            ppu.render();
        }
    }

    return 0;
}