#include <assert.h>
#include <iostream>

#include "GameBoy.h"


using namespace std;


GameBoy::GameBoy() : _memory(Memory{}), _cpu(Cpu{ _memory }), _ppu(Ppu{ _memory }),
                     _joypad(Joypad{ _memory }), _timer(Timer{ _memory }) {}


void GameBoy::load_rom(const u8* rom) {
    // No banking has been implemented yet.
    //assert(rom[0x147] == 0x00);

	_memory.load_rom(rom);
}


// This function should be called 60 times a second (60 fps)
void GameBoy::update_frame(SDL_Renderer* renderer) {
    const unsigned int CYCLES_PER_FRAME = 69905;
    
    // Total number of (clock) cycles during this frame
    unsigned int total_cycles = 0;

    while (total_cycles < CYCLES_PER_FRAME) {
        // TODO: Logic with HALT may have to be fixed
        unsigned int cycles;

        if (_cpu.is_on) {
            cycles = _cpu.execute_next();

            assert(cycles % 4 == 0);
        }
        else {
            cycles = 4;
        }

        _timer.update(cycles);
        _ppu.update(cycles);

        // Update joypad input register (FF00 - JOYP)
        _joypad.update_joyp_register();

        // TODO: What about cycles from interrupt?
        _cpu.handle_interrupt();

        total_cycles += cycles;
    }

    _ppu.render(renderer);
}

// This function is called when a button is pressed.
void GameBoy::on_button_pressed(Button button) {
    _joypad.on_button_pressed(button);
}

// This function is called when a button is released.
void GameBoy::on_button_released(Button button) {
    _joypad.on_button_released(button);
}
