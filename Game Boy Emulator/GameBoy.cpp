#include <assert.h>
#include <iostream>

#include "GameBoy.h"


using namespace std;


GameBoy::GameBoy() : _memory(Memory{}), _cpu(Cpu{ _memory }), _ppu(Ppu{ _memory }), 
                     _timer(Timer{}) {
}


void GameBoy::load_rom(const u8* rom) {
    // No banking has been implemented yet.
    //assert(rom[0x147] == 0x00);

	_memory.load_rom(rom);
}


// This function should be called 60 times a second (60 fps)
void GameBoy::update_frame() {
    const unsigned int CYCLES_PER_FRAME = 69905;
    
    // Total number of (clock) cycles during this frame
    unsigned int total_cycles = 0;

    while (total_cycles < CYCLES_PER_FRAME) {
        unsigned int cycles = _cpu.execute_next();
        
        assert(cycles % 4 == 0);

        _timer.update(cycles);
        _ppu.update(cycles);
        _cpu.handle_interrupt();

        total_cycles += cycles;
    }

    _ppu.render();
}