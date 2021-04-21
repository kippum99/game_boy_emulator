#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include <SDL.h>

#include "GameBoy.h"


using namespace std;


int main(int argc, char* args[]) {
    u8* cartridge_memory = new u8[0x200000];

    FILE* rom_file;
    rom_file = fopen("roms/tetris.gb", "rb");

    fread(cartridge_memory, 1, 0x200000, rom_file);
    fclose(rom_file);

    GameBoy gameboy{};
    gameboy.load_rom(cartridge_memory);

    // Initialize SDL
    SDL_Event event;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_CreateWindowAndRenderer(160, 144, 0, &window, &renderer);

    bool quit = false;

    while (!quit) {
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                quit = true;
                false;
                break;
            }
        }
        gameboy.update_frame(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}