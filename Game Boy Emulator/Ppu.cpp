#include <cassert>
#include <iostream>
#include <list>

#include "Ppu.h"
#include "utils.h"


using namespace std;


Ppu::Ppu(Memory& memory) : _memory(memory) {
    _curr_scanline_cycles = 0;
    _window_line = 0;

    _pixel_matrix = new u2[144 * 160]{ 0 };
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
    
    // Used so that LY=LYC is checked only when it is a new scanline
    bool is_new_scanline = false;

    if (_curr_scanline_cycles >= CYCLES_PER_SCANLINE) {
        // Draw current scanline before moving on to the next scaline
        unsigned int curr_scanline = _memory.read(0xFF44);

        if (curr_scanline < 144) {
            _draw_scanline(curr_scanline);
        }

        // Move to the next scanline
        _curr_scanline_cycles %= CYCLES_PER_SCANLINE;

        unsigned int next_scanline = (curr_scanline + 1) % 154;

        // Check if we just entered VBlank mode
        if (next_scanline == 144) {
            _memory.request_interrupt(0);
        }

        // Update LY register value
        _memory.write(0xFF44, next_scanline);

        is_new_scanline = true;
    }

    // TODO: Is it ok to check LYC == LY after updating LY? or do it before and after?
    // This is probably ok..
    _set_lcd_status(is_new_scanline);
}


void Ppu::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    for (int y = 0; y < 144; y++) {
        for (int x = 0; x < 160; x++) {
            u2 color_value = _pixel_matrix[y * 160 + x];

            switch (color_value) {
            case 0:     // White
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                break;
            case 1:     // Light gray
                SDL_SetRenderDrawColor(renderer, 170, 170, 170, 255);
                break;
            case 2:     // Dark gray
                SDL_SetRenderDrawColor(renderer, 85, 85, 85, 255);
                break;
            case 3:     // Black
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                break;
            }

            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

    SDL_RenderPresent(renderer);
}


void Ppu::_set_lcd_status(const bool is_new_scanline) {
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
        
        // Reset window internal line counter
        _window_line = 0;
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

    // Update coincidence flag and request STAT interrupt if needed
    if (is_new_scanline) {
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
    }

    // Update STAT register value
    stat = (stat & 0b11111100) + curr_mode;
    _memory.write(0xFF41, stat);
}

void Ppu::_draw_scanline(const unsigned int y) {
    // TODO: Check if window display is enabled (bit 5), and check if bg and window are enabled (bit 0).

    // Get LCDC register value 
    const u8 lcdc = _memory.read(0xFF40);

    // Check if backround and window display is enabled
    if (get_bit(lcdc, 0)) {
        _draw_background(y, lcdc);
    }

    // Check if object display is enabled
    if (get_bit(lcdc, 1)) {
        _draw_objects(y, lcdc);
    }
}

void Ppu::_draw_background(const unsigned int y, const u8 lcdc) {
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

    // Get WY and WX
    u8 wy = _memory.read(0xFF4A);
    u8 wx = _memory.read(0xFF4B);

    // The following is true if window display is enabled and the current scanline
    // (or part of it) is in window
    bool is_using_window = get_bit(lcdc, 5) && (y >= wy) && (wx >= 0 && wx <= 166);

    // Draw scanline
    for (int x = 0; x < 160; x++) {
        // Check if background or window should be displayed at this pixel
        bool is_in_window = is_using_window && (x >= (i8)(wx - 7));

        // Get address of background or window tile map to use
        u16 tile_map_addr;

        // Get x, y coordinate of the pixel in window or background space (256 x 256)
        unsigned int translated_x;
        unsigned int translated_y;

        if (is_in_window) {
            if (get_bit(lcdc, 6)) {
                tile_map_addr = 0x9C00;
            }
            else {
                tile_map_addr = 0x9800;
            }

            // Find the x, y coordinate of the pixel in window space
            translated_x = x - (wx - 7);
            //translated_y = y - wy;
            translated_y = _window_line;
        }
        else {
            if (get_bit(lcdc, 3)) {
                tile_map_addr = 0x9C00;
            }
            else {
                tile_map_addr = 0x9800;
            }

            // Find the x, y coordinate of the pixel in background space
            translated_x = (scroll_x + x) % 256;
            translated_y = (scroll_y + y) % 256;
        }

        // Get tile number (before mapping), ranging from 0 to 1023
        int tile_num = 32 * (int)(translated_y / 8) + (int)(translated_x / 8);

        // Get mapped tile number 
        u8 mapped_tile_num = _memory.read(tile_map_addr + tile_num);

        // Get row / col within tile 
        u3 tile_row = translated_y % 8;
        u3 tile_col = translated_x % 8;
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

        // Get color value from background palette (BGP)
        u8 bgp = _memory.read(0xFF47);
        u2 color_value = 0b11 & (bgp >> (color_num * 2));

        // Draw pixel
        _pixel_matrix[y * 160 + x] = color_value;
    }

    // Increment window scanline counter
    if (is_using_window) {
        _window_line++;
    }
}


struct Sprite {
    int x_pos;					// x position of the leftmost part of the sprite (may be negative)
    int y_pos;                  // y position of the topmost part of the sprite (may be negative)
    unsigned int sprite_index;	// Ranges from 0 to 39
    u8 tile_num;
    u8 attributes;
};


// Returns true if first has a higher priority than second, false otherwise.
bool compare_sprite_priority(const Sprite& first, const Sprite& second) {
    if (first.x_pos == second.x_pos) {
        return (first.sprite_index < second.sprite_index);
    }

    return (first.x_pos < second.x_pos);
}


void Ppu::_draw_objects(const unsigned int y, const u8 lcdc) {
    // Check if sprites are 8x8 tiles or 8x16 tiles
    unsigned int sprite_size_y;

    if (get_bit(lcdc, 2)) {
        sprite_size_y = 16;
    }
    else {
        sprite_size_y = 8;
    }

    // Scan all 40 sprites and find the first 10 sprites in the current scanline,
    // discarding unused sprites in the scanline

    // Number of sprites in this scanline
    unsigned int num_sprites = 0;

    // List of 10 Sprite objects to be displayed in the scanline
    list<Sprite> sprites;

    for (unsigned int sprite_index = 0; sprite_index < 40; sprite_index++) {
        // Address for sprite attribute
        u16 sprite_addr = 0xFE00 + sprite_index * 4;

        // x, y positions of the top left corner of the sprite
        int sprite_y = _memory.read(sprite_addr) - 16;
        int sprite_x = _memory.read(sprite_addr + 1) - 8;
        u8 tile_num = _memory.read(sprite_addr + 2);
        u8 attributes = _memory.read(sprite_addr + 3);

        if (y >= sprite_y && y < sprite_y + sprite_size_y) {
            if (num_sprites < 10) {
                num_sprites++;

                // Create a Sprite object and add to sprites list
                sprites.push_back(Sprite{ sprite_x, sprite_y, sprite_index, tile_num, attributes });
            }
            else {
                // Discard the sprite - we cannot display this sprite
                // Set Y Position to 0 to hide the sprite.
                _memory.write(sprite_addr, 0);
            }
        }
    }

    // Sort sprites in order of highest priority to lowest priority
    sprites.sort(compare_sprite_priority);

    // Temporary array of length 160 to hold sprite pixels drawn in the scanline
    // We need to store both color numbers and color values because there is no way 
    // to differentiate color number 0 (transparent) from color value 0 (white color)
    // TODO: color_numbers can just be 0 or 1 indicating transparency (whether it was drawn)
    u2 pixel_color_numbers[160]{ 0 };
    u2 pixel_color_values[160]{ 0 };
    
    // Array of length 160 that reflects the OBJ-to-BG priority attribute of 
    // the highest priority sprite (0 means draw over BG, 1 means do not)
    bool bg_priority[160]{ 0 };

    // Draw sprites, respecting the z-axis priority
    for (const Sprite& sprite : sprites) {
        // Get sprite attributes
        bool is_behind_background = get_bit(sprite.attributes, 7);
        bool y_flip = get_bit(sprite.attributes, 6);
        bool x_flip = get_bit(sprite.attributes, 5);
        bool palette_num = get_bit(sprite.attributes, 4);     // OBP0 or OBP1?

        // Get the scanline's corresponding row within tile 
        u3 tile_row;

        if (!y_flip) {
            tile_row = y - sprite.y_pos;
        }
        else {
            tile_row = (sprite_size_y - 1) - (y - sprite.y_pos);
        }

        // Get line address by properly finding tile_num and tile_row within 
        // upper or lower tile if the tile is 8x16.
        u8 tile_num;

        if (sprite_size_y == 8) {
            tile_num = sprite.tile_num;
        }
        else {
            if (tile_row < 8) {     // Upper tile
                tile_num = sprite.tile_num & 0b11111110;
            }
            else {                  // Lower tile
                tile_row = tile_row - 8;
                tile_num = sprite.tile_num | 1;
            }
        }
        u16 line_addr = 0x8000 + tile_num * 16 + tile_row * 2;

        // Render row
        for (u3 x = 0; x < 8; x++) {
            // Find col within tile
            u3 tile_col;

            if (!x_flip) {
                tile_col = x;
            }
            else {
                tile_col = 7 - x;
            }

            // Get x position in LCD
            int x_pos = sprite.x_pos + x;

            if (x_pos < 0 || x_pos >= 160) {
                continue;
            }

            // Don't draw over a sprite with a higher priority
            if (pixel_color_numbers[x_pos] != 0) {
                continue;
            }

            // The current sprite is the highest priority sprite at this pixel.
            // Set the OBJ-to-BG priority for the pixel
            bg_priority[x_pos] = is_behind_background;

            // Get color number
            u3 pixel_bit = 7 - tile_col;

            // Get low and high bits of color number 
            bool color_number_low = get_bit(_memory.read(line_addr), pixel_bit);
            bool color_number_high = get_bit(_memory.read(line_addr + 1), pixel_bit);

            u2 color_num = (color_number_high << 1) | color_number_low;

            // Color number 0 is transparent
            if (color_num != 0) {
                pixel_color_numbers[x_pos] = color_num;

                // Get color value from object palette (OBP0 or OBP1)
                u8 obp;

                if (palette_num == 0) {
                    obp = _memory.read(0xFF48);
                }
                else {
                    obp = _memory.read(0xFF49);
                }

                u2 color_value = 0b11 & (obp >> (color_num * 2));

                // Draw pixel
                pixel_color_values[x_pos] = color_value;
            }
        }
    }

    // Draw scanline, drawing sprites on top of background if they have priority over background
    for (int x = 0; x < 160; x++) {
        if (pixel_color_numbers[x] != 0) {
            // Draw over background if object has priority or if background has color 0
            if (bg_priority[x] == 0 || _pixel_matrix[y * 160 + x] == 0) {
                _pixel_matrix[y * 160 + x] = pixel_color_values[x];
            }
        }
    }
}
