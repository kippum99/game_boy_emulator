#include "GameBoy.h"

GameBoy::GameBoy() : _memory(Memory{}), _cpu(Cpu{ _memory }), _ppu(Ppu{ _memory }) {
}


void GameBoy::load_rom(const u8* rom) {
	_memory.load_rom(rom);
}


void GameBoy::update_frame() {
    const unsigned int CYCLES_PER_FRAME = 69905;
    
    unsigned int num_cycles = 0;

    while (num_cycles < CYCLES_PER_FRAME) {
        num_cycles += _cpu.execute_next();
    }

    if (_cpu._registers.get_pc() > 0x3000) {
        _ppu.render();
    }
}