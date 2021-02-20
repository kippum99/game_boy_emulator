#include <iostream>

#include <SDL.h>

#include "Ppu.h"
#include "utils.h"


using namespace std;


Ppu::Ppu(Memory& memory) : _memory(memory) {
	// Initialize SDL
    //SDL_Event event;
    //SDL_Window* window;
    //SDL_Renderer* renderer;
    //SDL_CreateWindowAndRenderer(160, 144, 0, &_window, &_renderer);
    SDL_CreateWindowAndRenderer(256, 256, 0, &_window, &_renderer);

    SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
    SDL_RenderClear(_renderer);

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
}


void Ppu::render() {
    // Get LCDC register value 
    const u8 lcdc = _memory.read(0xFF40);

    // Check which BG tile data map to use
    u16 bg_tile_map_addr;

    if (get_bit(lcdc, 3) == 0) {
        bg_tile_map_addr = 0x9800;
    }
    else {
        bg_tile_map_addr = 0x9C00;
    }

    // Check which BG & Window tile data table to use
    u16 tile_data_addr;

    if (get_bit(lcdc, 4) == 0) {
        tile_data_addr = 0x8800;
    }
    else {
        tile_data_addr = 0x8000;
    }
    
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);

    // Draw 256 x 256 pixels of background 
    for (int y = 0; y < 256; y++) {
        for (int x = 0; x < 256; x++) {
            // Get tile number (before mapping), ranging from 0 to 1023
            int tile_num = 32 * (int)(y / 8) + (int)(x / 8);

            // Get mapped tile number 
            u8 mapped_tile_num = _memory.read(bg_tile_map_addr + tile_num);

            // Get row / col within tile 
            u3 tile_row = y % 8;
            u3 tile_col = x % 8;
            u3 pixel_bit = 7 - tile_col;

            // Get line address
            u16 line_addr;

            if (tile_data_addr == 0x8000) {
                line_addr = 0x8000 + mapped_tile_num * 16 + tile_row * 2;
            }
            else {
                line_addr = 0x8800 + (i8)mapped_tile_num * 16 + tile_row * 2;
            }

            // Get low and high bits of color number 
            bool color_number_low = get_bit(_memory.read(line_addr), pixel_bit);
            bool color_number_high = get_bit(_memory.read(line_addr + 1), pixel_bit);

            // Get color number 
            u2 color_num = (color_number_high << 1) | color_number_low;

            // Draw pixel 
            if (color_num != 0) {
                SDL_RenderDrawPoint(_renderer, x, y);
            }

        }
    }

    SDL_RenderPresent(_renderer);

    //// Draw tile (i is row index and j is col index)
    //for (int i = 0; i < 8; i++) {
    //    u16 line_addr = 0x8210 + i * 2;

    //    // Low bits of color numbers for the line
    //    u8 color_numbers_low = _memory.read(line_addr);

    //    // High bits of color numbers for the line
    //    u8 color_numbers_high = _memory.read(line_addr + 1);

    //    for (int j = 0; j < 8; j++) {
    //        u3 pixel_bit = 7 - j;
    //        u2 color_num = ((get_bit(color_numbers_high, pixel_bit) << 1)
    //            | get_bit(color_numbers_low, pixel_bit));
    //        //printf("%02X ", color_num);

    //        if (color_num != 0) {
    //            SDL_RenderDrawPoint(_renderer, j, i);
    //        }
    //    }

    //    SDL_RenderPresent(_renderer);

    //    //printf("\n");
    //}
}