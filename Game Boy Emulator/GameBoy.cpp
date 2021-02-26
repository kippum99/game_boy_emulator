#include <iostream>

#include "GameBoy.h"


using namespace std;


GameBoy::GameBoy() : _memory(Memory{}), _cpu(Cpu{ _memory }), _ppu(Ppu{ _memory }), 
                     _timer(Timer{}) {
}


void GameBoy::load_rom(const u8* rom) {
	_memory.load_rom(rom);
}


// This function should be called 60 times a second (60 fps)
void GameBoy::update_frame() {
    const unsigned int CYCLES_PER_FRAME = 69905;
    
    // Total number of cycles during this frame
    unsigned int total_cycles = 0;

    while (total_cycles < CYCLES_PER_FRAME) {
        unsigned int cycles = _cpu.execute_next();

        _timer.update(cycles);
        //_ppu.update(cycles);
        _cpu.handle_interrupt();

        total_cycles += cycles;
    }

    if (_cpu._registers.get_pc() > 0x3000) {
        _ppu.render();
    }
}