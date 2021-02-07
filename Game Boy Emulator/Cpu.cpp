#include <iostream>

#include "Cpu.h"

#include "utils.h"


using namespace std;


Cpu::Cpu(Memory& memory) : _memory(memory), _registers(Registers{}) {
    // Power up - initialize program counter and stack pointer 
    _registers.set_pc(0x100);
    _registers.set_sp(0xFFFE);
}


void Cpu::run_rom(u8* rom) {
    int counter = 0;

    while (true) {
        u8 opcode = rom[_registers.get_pc()];
        _registers.inc_pc();

        printf("%X ", opcode);

        u8 next_u8;
        u16 next_u16;

        switch (opcode) {
        case 0x00:
            //cout << "NOP" << endl;
            break;
        case 0x01:
            cout << "LD BC,u16" << endl;

            _registers.set_bc(_read16(rom));

            break;
        case 0x06:
            cout << "LD B,u8" << endl;

            _registers.set_b(_read8(rom));

            break;
        case 0x11:
            cout << "LD DE,u16" << endl;

            _registers.set_de(_read16(rom));

            break;
        case 0x21:
            cout << "LD HL,u16" << endl;

            _registers.set_hl(_read16(rom));

            break;
        case 0x31:
            cout << "LD SP,u16" << endl;

            _registers.set_sp(_read16(rom));

            break;
        case 0x3C:
            cout << "INC A" << endl;
            break;
        case 0xC0:
            cout << "RET NZ" << endl;

            if (_registers.get_flag_z() == 0) {
                u8 lsb = _memory.read(_registers.get_sp());
                _registers.inc_sp();
                u8 msb = _memory.read(_registers.get_sp());
                _registers.inc_sp();
                _registers.set_pc(get_u16(lsb, msb));
            }

            break;
        case 0xC3:
            cout << "JP u16" << endl;

            _registers.set_pc(_read16(rom));
            //cout << "Set PC to " << u16_to_binary(_registers.get_pc()) << endl;

            break;
        case 0xC8:
            cout << "RET Z" << endl;

            if (_registers.get_flag_z() == 1) {
                u8 lsb = _memory.read(_registers.get_sp());
                _registers.inc_sp();
                u8 msb = _memory.read(_registers.get_sp());
                _registers.inc_sp();
                _registers.set_pc(get_u16(lsb, msb));
            }

            break;
        case 0xC9:
        {
            cout << "RET" << endl;

            u8 lsb = _memory.read(_registers.get_sp());
            _registers.inc_sp();
            u8 msb = _memory.read(_registers.get_sp());
            _registers.inc_sp();
            _registers.set_pc(get_u16(lsb, msb));

            break;
        }
        case 0xCD:
            cout << "CALL u16" << endl;

            next_u16 = _read16(rom);

            // Save current pc value in stack
            _registers.dec_sp();
            _memory.write(_registers.get_sp(), msb(_registers.get_pc()));
            _registers.dec_sp();
            _memory.write(_registers.get_sp(), lsb(_registers.get_pc()));

            _registers.set_pc(next_u16);

            break;
        case 0xCE:
            break;
        case 0xD0:
            cout << "RET NC" << endl;

            if (_registers.get_flag_c() == 0) {
                u8 lsb = _memory.read(_registers.get_sp());
                _registers.inc_sp();
                u8 msb = _memory.read(_registers.get_sp());
                _registers.inc_sp();
                _registers.set_pc(get_u16(lsb, msb));
            }

            break;
        case 0xD8:
            cout << "RET C" << endl;

            if (_registers.get_flag_c() == 1) {
                u8 lsb = _memory.read(_registers.get_sp());
                _registers.inc_sp();
                u8 msb = _memory.read(_registers.get_sp());
                _registers.inc_sp();
                _registers.set_pc(get_u16(lsb, msb));
            }

            break;
        case 0xE9:
            cout << "JP HL" << endl;
            _registers.set_pc(_registers.get_hl());
            //cout << "Set PC to " << _registers.get_pc() << endl;
            break;
        case 0xF0:
            cout << "LD A,(FF00+u8)" << endl;

            next_u8 = _read8(rom);
            _registers.set_a(get_u16(next_u8, 0xFF));

            break;
        case 0xF3:
            cout << "DI" << endl;
            // TODO
            break;
        case 0xFE:
        {
            cout << "CP A,u8" << endl;

            _cp(_registers.get_a(), _read8(rom));

            break;
        }
        case 0xCB:  // Extended opcode
        {
            u8 extended_opcode = rom[_registers.get_pc()];
            _registers.inc_pc();

            printf("%X ", extended_opcode);

            switch (extended_opcode) {
            case 0x7E:
                cout << "BIT 7,(HL)" << endl;

                if (((_memory.read(_registers.get_hl()) >> 7) & 1) == 0) {
                    _registers.set_flag_z();
                }
                else {
                    _registers.clear_flag_z();
                }

                _registers.clear_flag_n();
                _registers.set_flag_h();

                break;
            default:
                cout << "Unrecognized (extended) opcode" << endl;
            }

            break;
        }
        default:
            cout << "Unrecognized opcode" << endl;
        }

        if (counter++ == 20) {
            break;
        }
    }
}


/* For reading ROM file directly - must replace later */

// Reads 8 bits from ROM file (pc)
u8 Cpu::_read8(const u8* rom) {
    u8 u8 = rom[_registers.get_pc()];
    _registers.inc_pc();

    return u8;
}

// Reads 16 bits from ROM file
u16 Cpu::_read16(const u8* rom) {
    // Read the next two bytes
    u8 lsb = rom[_registers.get_pc()];
    _registers.inc_pc();
    u8 msb = rom[_registers.get_pc()];
    _registers.inc_pc();

    // Get the 16-bit address
    u16 u16 = msb << 8 | lsb;

    return u16;
}


/* Instruction helper functions */

void Cpu::_cp(const u8 val1, const u8 val2) {
    char res = val1 - val2;

    if (res == 0) {
        _registers.set_flag_z();
    }
    else {
        _registers.clear_flag_z();
    }

    _registers.set_flag_n();

    if ((val1 & 0xF) - (val2 & 0xF) < 0) {
        _registers.set_flag_h();
    }
    else {
        _registers.clear_flag_h();
    }

    if (res < 0) {
        _registers.set_flag_c();
    }
    else {
        _registers.clear_flag_c();
    }
}