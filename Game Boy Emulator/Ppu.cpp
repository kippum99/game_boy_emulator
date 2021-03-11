#include <iostream>

#include "Ppu.h"
#include "utils.h"


using namespace std;


Ppu::Ppu(Memory& memory) : _memory(memory) {
    _curr_scanline_cycles = 0;

    _pixel_matrix = new u3[144 * 160]{ 0 };

	// Initialize SDL
    //SDL_Event event;
    //SDL_Window* window;
    //SDL_Renderer* renderer;
    SDL_CreateWindowAndRenderer(160, 144, 0, &_window, &_renderer);
    //SDL_CreateWindowAndRenderer(256, 256, 0, &_window, &_renderer);

    //SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
    //SDL_RenderClear(_renderer);

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


void Ppu::update(const unsigned int cycles) {
    // Get LCDC register value and check if LCD is enabled 
    const u8 lcdc = _memory.read(0xFF40);
    
    if (get_bit(lcdc, 7) == 0) {
        // Reset scanline
        _curr_scanline_cycles = 0;
        _memory.write(0xFF44, 0);

        // Set STAT mode to 1
        u8 stat = _memory.read(0xFF41);
        _memory.write(0xFF41, set_bit(stat & 0b11111100, 0, 1));

        return;
    }

    _curr_scanline_cycles += cycles;

    if (_curr_scanline_cycles >= CYCLES_PER_SCANLINE) {
        // Draw current scanline before moving on to the next scaline
        unsigned int curr_scanline = _memory.read(0xFF44);

        if (curr_scanline < 144) {
            _draw_scanline(curr_scanline);
        }

        // Move to the next scanline
        _curr_scanline_cycles %= CYCLES_PER_SCANLINE;

        // Update LY register value
        _memory.write(0xFF44, (curr_scanline + 1) % 154);
    }

    // TODO: Is it ok to check LYC == LY after updating LY? or do it before and after?
    // This is probably ok..
    _set_lcd_status();
}


void Ppu::render() {
    SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
    SDL_RenderClear(_renderer);

    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);

    for (int y = 0; y < 144; y++) {
        for (int x = 0; x < 160; x++) {
            if (_pixel_matrix[y * 160 + x] == 1) {
                SDL_RenderDrawPoint(_renderer, x, y);
                //_pixel_matrix[y * 160 + x] = 0;
            }
        }
    }

    SDL_RenderPresent(_renderer);
}


void Ppu::_set_lcd_status() {
    // Get LY register value and set STAT
    u8 ly = _memory.read(0xFF44);
    u8 stat = _memory.read(0xFF41);
    u3 prev_mode = stat & 0b00000011;
    u3 curr_mode;
    bool is_request_enabled = false;

    if (ly >= 144) {
        // V-Blank (Mode 1)
        curr_mode = 1;
        is_request_enabled = get_bit(stat, 4);
    }
    else {
        // Check cycles elapsed during the current scanline to find which mode we're in
        if (_curr_scanline_cycles < 80) {
            // Mode 2 (OAM Interrupt)
            curr_mode = 2;
            is_request_enabled = get_bit(stat, 5);
        }
        else if (_curr_scanline_cycles < 252) {
            // Mode 3
            curr_mode = 3;
        }
        else {
            // Mode 0 (H-Blank Interrupt)
            curr_mode = 0;
            is_request_enabled = get_bit(stat, 3);
        }
    }

    // Request STAT interrupt if we entered a new mode
    if (prev_mode != curr_mode && is_request_enabled) {
        _memory.request_interrupt(1);
    }

    // Update coicidence flag and request STAT interrupt if needed
    u8 lyc = _memory.read(0xFF45);

    if (ly == lyc) {
        stat = set_bit(stat, 2, 1);

        if (get_bit(stat, 6)) {
            _memory.request_interrupt(1);
        }
    }
    else {
        stat = set_bit(stat, 2, 0);
    }

    // Update STAT register value
    stat = (stat & 0b11111100) + curr_mode;
    _memory.write(0xFF41, stat);
}


void Ppu::_draw_scanline(const unsigned int y) {
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
        tile_data_addr = 0x9000;
    }
    else {
        tile_data_addr = 0x8000;
    }

    // Get SCY and SCX
    u8 scroll_y = _memory.read(0xFF42);
    u8 scroll_x = _memory.read(0xFF43);

    // Draw scanline
    for (int x = 0; x < 144; x++) {
        // Find the x, y coordinate of the 256 x 256 background pixel to display
        unsigned int background_x = (scroll_x + x) % 256;
        unsigned int background_y = (scroll_y + y) % 256;

        // Get tile number (before mapping), ranging from 0 to 1023
        int tile_num = 32 * (int)(background_y / 8) + (int)(background_x / 8);

        // Get mapped tile number 
        u8 mapped_tile_num = _memory.read(bg_tile_map_addr + tile_num);

        if (mapped_tile_num == 0x9B) {
            cout << "check here" << endl;
        }

        // Get row / col within tile 
        u3 tile_row = background_y % 8;
        u3 tile_col = background_x % 8;
        u3 pixel_bit = 7 - tile_col;

        // Get line address
        u16 line_addr;

        if (tile_data_addr == 0x8000) {
            line_addr = 0x8000 + mapped_tile_num * 16 + tile_row * 2;
        }
        else {
            line_addr = 0x9000 + (i8)mapped_tile_num * 16 + tile_row * 2;
        }

        // Get low and high bits of color number 
        bool color_number_low = get_bit(_memory.read(line_addr), pixel_bit);
        bool color_number_high = get_bit(_memory.read(line_addr + 1), pixel_bit);

        // Get color number 
        u2 color_num = (color_number_high << 1) | color_number_low;

        // Draw pixel
        _pixel_matrix[y * 160 + x] = (color_num != 0);
    }
}