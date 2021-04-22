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
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_RIGHT:
                    gameboy.on_button_pressed(Button::right);
                    break;
                case SDLK_LEFT:
                    gameboy.on_button_pressed(Button::left);
                    break;
                case SDLK_UP:
                    gameboy.on_button_pressed(Button::up);
                    break;
                case SDLK_DOWN:
                    gameboy.on_button_pressed(Button::down);
                    break;
                case SDLK_s:
                    gameboy.on_button_pressed(Button::a);
                    break;
                case SDLK_a:
                    gameboy.on_button_pressed(Button::b);
                    break;
                case SDLK_LSHIFT:
                case SDLK_RSHIFT:
                    gameboy.on_button_pressed(Button::select);
                    break;
                case SDLK_RETURN:
                    gameboy.on_button_pressed(Button::start);
                    break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_RIGHT:
                    gameboy.on_button_released(Button::right);
                    break;
                case SDLK_LEFT:
                    gameboy.on_button_released(Button::left);
                    break;
                case SDLK_UP:
                    gameboy.on_button_released(Button::up);
                    break;
                case SDLK_DOWN:
                    gameboy.on_button_released(Button::down);
                    break;
                case SDLK_s:
                    gameboy.on_button_released(Button::a);
                    break;
                case SDLK_a:
                    gameboy.on_button_released(Button::b);
                    break;
                case SDLK_LSHIFT:
                case SDLK_RSHIFT:
                    gameboy.on_button_released(Button::select);
                    break;
                case SDLK_RETURN:
                    gameboy.on_button_released(Button::start);
                    break;
                }
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