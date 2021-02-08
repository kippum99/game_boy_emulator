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

    //cout.setstate(ios_base::failbit);

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
        case 0x02:
            cout << "LD (BC),A" << endl;

            _memory.write(_registers.get_bc(), _registers.get_a());

            break;
        case 0x04:
            cout << "INC B" << endl;

            _registers.set_b(_inc(_registers.get_b()));

            break;
        case 0x05:
        {
            cout << "DEC B" << endl;

            u8 res = _registers.get_b() - 1;
            _registers.set_b(res);

            _registers.set_flag_z(res == 0);
            _registers.set_flag_n(1);
            _registers.set_flag_h((res & 0x0F) == 0x0F);

            break;
        }
        case 0x06:
            cout << "LD B,u8" << endl;

            _registers.set_b(_read8(rom));

            break;
        case 0x08:
        {
            cout << "LD (u16),SP" << endl;

            next_u16 = _read16(rom);

            u16 sp_val = _registers.get_sp();
            _memory.write(next_u16, lsb(sp_val));
            _memory.write(next_u16 + 1, msb(sp_val));

            break;
        }
        case 0x0A:
            cout << "LD A,(BC)" << endl;

            _registers.set_a(_memory.read(_registers.get_bc()));

            break;
        case 0x0C:
            cout << "INC C" << endl;

            _registers.set_c(_inc(_registers.get_c()));

            break;
        case 0x0D:
        {
            cout << "DEC C" << endl;

            u8 res = _registers.get_c() - 1;
            _registers.set_c(res);

            _registers.set_flag_z(res == 0);
            _registers.set_flag_n(1);
            _registers.set_flag_h((res & 0x0F) == 0x0F);

            break;
        }
        case 0x0E:
            cout << "LD C,u8" << endl;

            _registers.set_c(_read8(rom));

            break;
        case 0x11:
            cout << "LD DE,u16" << endl;

            _registers.set_de(_read16(rom));

            break;
        case 0x12:
            cout << "LD (DE),A" << endl;

            _memory.write(_registers.get_de(), _registers.get_a());

            break;
        case 0x14:
            cout << "INC D" << endl;

            _registers.set_d(_inc(_registers.get_d()));

            break;
        case 0x15:
        {
            cout << "DEC D" << endl;

            u8 res = _registers.get_d() - 1;
            _registers.set_d(res);

            _registers.set_flag_z(res == 0);
            _registers.set_flag_n(1);
            _registers.set_flag_h((res & 0x0F) == 0x0F);

            break;
        }
        case 0x16:
            cout << "LD D,u8" << endl;

            _registers.set_d(_read8(rom));

            break;
        case 0x18:
            cout << "JR i8" << endl;

            _registers.set_pc(_registers.get_pc() + (i8)_read8(rom));

            break;
        case 0x1A:
            cout << "LD A,(DE)" << endl;

            _registers.set_a(_memory.read(_registers.get_de()));

            break;
        case 0x1C:
            cout << "INC E" << endl;

            _registers.set_e(_inc(_registers.get_e()));

            break;
        case 0x1D:
        {
            cout << "DEC E" << endl;

            u8 res = _registers.get_e() - 1;
            _registers.set_e(res);

            _registers.set_flag_z(res == 0);
            _registers.set_flag_n(1);
            _registers.set_flag_h((res & 0x0F) == 0x0F);

            break;
        }
        case 0x1E:
            cout << "LD E,u8" << endl;

            _registers.set_e(_read8(rom));

            break;
        case 0x20:
            cout << "JR NZ,i8" << endl;

            next_u8 = _read8(rom);

            if (_registers.get_flag_z() == 0) {
                _registers.set_pc(_registers.get_pc() + (i8)next_u8);
            }

            break;
        case 0x21:
            cout << "LD HL,u16" << endl;

            _registers.set_hl(_read16(rom));

            break;
        case 0x24:
            cout << "INC H" << endl;

            _registers.set_h(_inc(_registers.get_h()));

            break;
        case 0x25:
        {
            cout << "DEC H" << endl;

            u8 res = _registers.get_h() - 1;
            _registers.set_h(res);

            _registers.set_flag_z(res == 0);
            _registers.set_flag_n(1);
            _registers.set_flag_h((res & 0x0F) == 0x0F);

            break;
        }
        case 0x26:
            cout << "LD H,u8" << endl;

            _registers.set_h(_read8(rom));

            break;
        case 0x28:
            cout << "JR Z,i8" << endl;

            next_u8 = _read8(rom);

            if (_registers.get_flag_z() == 1) {
                _registers.set_pc(_registers.get_pc() + (i8)next_u8);
            }

            break;
        case 0x2A:
            cout << "LD A,(HL+)" << endl;

            _registers.set_a(_memory.read(_registers.get_hl()));
            _registers.inc_hl();

            break;
        case 0x2C:
            cout << "INC L" << endl;

            _registers.set_l(_inc(_registers.get_l()));

            break;
        case 0x2D:
        {
            cout << "DEC L" << endl;

            u8 res = _registers.get_l() - 1;
            _registers.set_l(res);

            _registers.set_flag_z(res == 0);
            _registers.set_flag_n(1);
            _registers.set_flag_h((res & 0x0F) == 0x0F);

            break;
        }
        case 0x2E:
            cout << "LD L,u8" << endl;

            _registers.set_l(_read8(rom));

            break;
        case 0x2F:
            cout << "CPL" << endl;

            _registers.set_a(~_registers.get_a());
            _registers.set_flag_n(1);
            _registers.set_flag_h(1);

            break;
        case 0x30:
            cout << "JR NC,i8" << endl;

            next_u8 = _read8(rom);

            if (_registers.get_flag_c() == 0) {
                _registers.set_pc(_registers.get_pc() + (i8)next_u8);
            }

            break;
        case 0x31:
            cout << "LD SP,u16" << endl;

            _registers.set_sp(_read16(rom));

            break;
        case 0x34:
        {
            cout << "INC (HL)" << endl;

            u16 hl_addr = _registers.get_hl();
            _memory.write(hl_addr, _inc(_memory.read(hl_addr)));

            break;
        }
        case 0x35:
        {
            cout << "DEC (HL)" << endl;

            u8 res = _memory.read(_registers.get_hl()) - 1;
            _memory.write(_registers.get_hl(), res);

            _registers.set_flag_z(res == 0);
            _registers.set_flag_n(1);
            _registers.set_flag_h((res & 0x0F) == 0x0F);

            break;
        }
        case 0x36:
            cout << "LD (HL),u8" << endl;

            _memory.write(_registers.get_hl(), _read8(rom));

            break;
        case 0x38:
            cout << "JR C,i8" << endl;

            next_u8 = _read8(rom);

            if (_registers.get_flag_c() == 1) {
                _registers.set_pc(_registers.get_pc() + (i8)next_u8);
            }

            break;
        case 0x3C:
            cout << "INC A" << endl;

            _registers.set_a(_inc(_registers.get_a()));

            break;
        case 0x3D:
        {
            cout << "DEC A" << endl;

            u8 res = _registers.get_a() - 1;
            _registers.set_a(res);

            _registers.set_flag_z(res == 0);
            _registers.set_flag_n(1);
            _registers.set_flag_h((res & 0x0F) == 0x0F);

            break;
        }
        case 0x3E:
            cout << "LD A,u8" << endl;

            next_u8 = _read8(rom);
            _registers.set_a(next_u8);

            break;
        case 0x40:
            cout << "LD B,B" << endl;

            // NOP

            break;
        case 0x41:
            cout << "LD B,C" << endl;

            _registers.set_b(_registers.get_c());

            break;
        case 0x42:
            cout << "LD B,D" << endl;

            _registers.set_b(_registers.get_d());

            break;
        case 0x43:
            cout << "LD B,E" << endl;

            _registers.set_b(_registers.get_e());

            break;
        case 0x44:
            cout << "LD B,H" << endl;

            _registers.set_b(_registers.get_h());

            break;
        case 0x45:
            cout << "LD B,L" << endl;

            _registers.set_b(_registers.get_l());

            break;
        case 0x46:
            cout << "LD B,(HL)" << endl;

            _registers.set_b(_memory.read(_registers.get_hl()));

            break;
        case 0x47:
            cout << "LD B,A" << endl;

            _registers.set_b(_registers.get_a());

            break;
        case 0x48:
            cout << "LD C,B" << endl;

            _registers.set_c(_registers.get_b());

            break;
        case 0x49:
            cout << "LD C,C" << endl;

            // NOP

            break;
        case 0x4A:
            cout << "LD C,D" << endl;

            _registers.set_c(_registers.get_d());

            break;
        case 0x4B:
            cout << "LD C,E" << endl;

            _registers.set_c(_registers.get_e());

            break;
        case 0x4C:
            cout << "LD C,H" << endl;

            _registers.set_c(_registers.get_h());

            break;
        case 0x4D:
            cout << "LD C,L" << endl;

            _registers.set_c(_registers.get_l());

            break;
        case 0x4E:
            cout << "LD C,(HL)" << endl;

            _registers.set_c(_memory.read(_registers.get_hl()));

            break;
        case 0x4F:
            cout << "LD C,A" << endl;

            _registers.set_c(_registers.get_a());

            break;
        case 0x50:
            cout << "LD D,B" << endl;

            _registers.set_d(_registers.get_b());

            break;
        case 0x51:
            cout << "LD D,C" << endl;

            _registers.set_d(_registers.get_c());

            break;
        case 0x52:
            cout << "LD D,D" << endl;

            // NOP

            break;
        case 0x53:
            cout << "LD D,E" << endl;

            _registers.set_d(_registers.get_e());

            break;
        case 0x54:
            cout << "LD D,H" << endl;

            _registers.set_d(_registers.get_h());

            break;
        case 0x55:
            cout << "LD D,L" << endl;

            _registers.set_d(_registers.get_l());

            break;
        case 0x56:
            cout << "LD D,(HL)" << endl;

            _registers.set_d(_memory.read(_registers.get_hl()));

            break;
        case 0x57:
            cout << "LD D,A" << endl;

            _registers.set_d(_registers.get_a());

            break;
        case 0x58:
            cout << "LD E,B" << endl;

            _registers.set_e(_registers.get_b());

            break;
        case 0x59:
            cout << "LD E,C" << endl;

            _registers.set_e(_registers.get_c());

            break;
        case 0x5A:
            cout << "LD E,D" << endl;

            _registers.set_e(_registers.get_d());

            break;
        case 0x5B:
            cout << "LD E,E" << endl;

            // NOP

            break;
        case 0x5C:
            cout << "LD E,H" << endl;

            _registers.set_e(_registers.get_h());

            break;
        case 0x5D:
            cout << "LD E,L" << endl;

            _registers.set_e(_registers.get_l());

            break;
        case 0x5E:
            cout << "LD E,(HL)" << endl;

            _registers.set_e(_memory.read(_registers.get_hl()));

            break;
        case 0x5F:
            cout << "LD E,A" << endl;

            _registers.set_e(_registers.get_a());

            break;
        case 0x60:
            cout << "LD H,B" << endl;

            _registers.set_h(_registers.get_b());

            break;
        case 0x61:
            cout << "LD H,C" << endl;

            _registers.set_h(_registers.get_c());

            break;
        case 0x62:
            cout << "LD H,D" << endl;

            _registers.set_h(_registers.get_d());

            break;
        case 0x63:
            cout << "LD H,E" << endl;

            _registers.set_h(_registers.get_e());

            break;
        case 0x64:
            cout << "LD H,H" << endl;

            // NOP

            break;
        case 0x65:
            cout << "LD H,L" << endl;

            _registers.set_h(_registers.get_l());

            break;
        case 0x66:
            cout << "LD H,(HL)" << endl;

            _registers.set_h(_memory.read(_registers.get_hl()));

            break;
        case 0x67:
            cout << "LD H,A" << endl;

            _registers.set_h(_registers.get_a());

            break;
        case 0x68:
            cout << "LD L,B" << endl;

            _registers.set_l(_registers.get_b());

            break;
        case 0x69:
            cout << "LD L,C" << endl;

            _registers.set_l(_registers.get_c());

            break;
        case 0x6A:
            cout << "LD L,D" << endl;

            _registers.set_l(_registers.get_d());

            break;
        case 0x6B:
            cout << "LD L,E" << endl;

            _registers.set_l(_registers.get_e());

            break;
        case 0x6C:
            cout << "LD L,H" << endl;

            _registers.set_l(_registers.get_h());

            break;
        case 0x6D:
            cout << "LD L,L" << endl;

            // NOP

            break;
        case 0x6E:
            cout << "LD L,(HL)" << endl;

            _registers.set_l(_memory.read(_registers.get_hl()));

            break;
        case 0x6F:
            cout << "LD L,A" << endl;

            _registers.set_l(_registers.get_a());

            break;
        case 0x70:
            cout << "LD (HL),B" << endl;

            _memory.write(_registers.get_hl(), _registers.get_b());

            break;
        case 0x71:
            cout << "LD (HL),C" << endl;

            _memory.write(_registers.get_hl(), _registers.get_c());

            break;
        case 0x72:
            cout << "LD (HL),D" << endl;

            _memory.write(_registers.get_hl(), _registers.get_d());

            break;
        case 0x73:
            cout << "LD (HL),E" << endl;

            _memory.write(_registers.get_hl(), _registers.get_e());

            break;
        case 0x74:
            cout << "LD (HL),H" << endl;

            _memory.write(_registers.get_hl(), _registers.get_h());

            break;
        case 0x75:
            cout << "LD (HL),L" << endl;

            _memory.write(_registers.get_hl(), _registers.get_l());

            break;
        case 0x77:
            cout << "LD (HL),A" << endl;

            _memory.write(_registers.get_hl(), _registers.get_a());

            break;
        case 0x78:
            cout << "LD A,B" << endl;

            _registers.set_a(_registers.get_b());

            break;
        case 0x79:
            cout << "LD A,C" << endl;

            _registers.set_a(_registers.get_c());

            break;
        case 0x7A:
            cout << "LD A,D" << endl;

            _registers.set_a(_registers.get_d());

            break;
        case 0x7B:
            cout << "LD A,E" << endl;

            _registers.set_a(_registers.get_e());

            break;
        case 0x7C:
            cout << "LD A,H" << endl;

            _registers.set_a(_registers.get_h());

            break;
        case 0x7D:
            cout << "LD A,L" << endl;

            _registers.set_a(_registers.get_l());

            break;
        case 0x7E:
            cout << "LD A,(HL)" << endl;

            _registers.set_a(_memory.read(_registers.get_hl()));

            break;
        case 0x7F:
            cout << "LD A,A" << endl;

            // NOP

            break;
        case 0xA8:
        {
            cout << "XOR A,B" << endl;

            u8 res = _registers.get_a() ^ _registers.get_b();
            _registers.set_a(res);

            _registers.set_flag_z(res == 0);
            _registers.set_flag_z(0);
            _registers.set_flag_h(0);
            _registers.set_flag_c(0);

            break;
        }
        case 0xA9:
        {
            cout << "XOR A,C" << endl;

            u8 res = _registers.get_a() ^ _registers.get_c();
            _registers.set_a(res);

            _registers.set_flag_z(res == 0);
            _registers.set_flag_z(0);
            _registers.set_flag_h(0);
            _registers.set_flag_c(0);

            break;
        }
        case 0xAA:
        {
            cout << "XOR A,D" << endl;

            u8 res = _registers.get_a() ^ _registers.get_d();
            _registers.set_a(res);

            _registers.set_flag_z(res == 0);
            _registers.set_flag_z(0);
            _registers.set_flag_h(0);
            _registers.set_flag_c(0);

            break;
        }
        case 0xAB:
        {
            cout << "XOR A,E" << endl;

            u8 res = _registers.get_a() ^ _registers.get_e();
            _registers.set_a(res);

            _registers.set_flag_z(res == 0);
            _registers.set_flag_z(0);
            _registers.set_flag_h(0);
            _registers.set_flag_c(0);

            break;
        }
        case 0xAC:
        {
            cout << "XOR A,H" << endl;

            u8 res = _registers.get_a() ^ _registers.get_h();
            _registers.set_a(res);

            _registers.set_flag_z(res == 0);
            _registers.set_flag_z(0);
            _registers.set_flag_h(0);
            _registers.set_flag_c(0);

            break;
        }
        case 0xAD:
        {
            cout << "XOR A,L" << endl;

            u8 res = _registers.get_a() ^ _registers.get_l();
            _registers.set_a(res);

            _registers.set_flag_z(res == 0);
            _registers.set_flag_z(0);
            _registers.set_flag_h(0);
            _registers.set_flag_c(0);

            break;
        }
        case 0xAE:
        {
            cout << "XOR A,(HL)" << endl;

            u8 res = _registers.get_a() ^ _memory.read(_registers.get_hl());
            _registers.set_a(res);
            
            _registers.set_flag_z(res == 0);
            _registers.set_flag_z(0);
            _registers.set_flag_h(0);
            _registers.set_flag_c(0);

            break;
        }
        case 0xAF:
            cout << "XOR A,A" << endl;

            _registers.set_a(0);

            _registers.set_flag_z(1);
            _registers.set_flag_z(0);
            _registers.set_flag_h(0);
            _registers.set_flag_c(0);

            break;
        case 0xB8:
            cout << "CP A,B" << endl;

            _cp(_registers.get_a(), _registers.get_b());

            break;
        case 0xB9:
            cout << "CP A,C" << endl;

            _cp(_registers.get_a(), _registers.get_c());

            break;
        case 0xBA:
            cout << "CP A,D" << endl;

            _cp(_registers.get_a(), _registers.get_d());

            break;
        case 0xBB:
            cout << "CP A,E" << endl;

            _cp(_registers.get_a(), _registers.get_e());

            break;
        case 0xBC:
            cout << "CP A,H" << endl;

            _cp(_registers.get_a(), _registers.get_h());

            break;
        case 0xBD:
            cout << "CP A,L" << endl;

            _cp(_registers.get_a(), _registers.get_l());

            break;
        case 0xBE:
            cout << "CP A,(HL)" << endl;

            _cp(_registers.get_a(), _memory.read(_registers.get_hl()));

            break;
        case 0xBF:
            cout << "CP A,A" << endl;

            _cp(_registers.get_a(), _registers.get_a());

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
        case 0xC1:
            cout << "POP BC" << endl;
            
            _registers.set_bc(_pop_stack());

            break;
        case 0xC2:
            cout << "JP NZ,u16" << endl;

            next_u16 = _read16(rom);

            if (_registers.get_flag_z() == 0) {
                _registers.set_pc(next_u16);
            }

            break;
        case 0xC3:
            cout << "JP u16" << endl;

            _registers.set_pc(_read16(rom));
            //cout << "Set PC to " << u16_to_binary(_registers.get_pc()) << endl;

            break;
        case 0xC5:
            cout << "PUSH BC" << endl;

            _push_stack(_registers.get_bc());

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
        case 0xCA:
            cout << "JP Z,u16" << endl;

            next_u16 = _read16(rom);

            if (_registers.get_flag_z() == 1) {
                _registers.set_pc(next_u16);
            }

            break;
        case 0xCD:
            cout << "CALL u16" << endl;

            next_u16 = _read16(rom);
                
            printf("Called function: %X ", next_u16);
            printf("pc: %X ", _registers.get_pc());

            return;
            // TODO: REMOVE THE FOLLOWING
            // Tile drawing function
            if (next_u16 == 0x0B79 || next_u16 == 0xC093) {
                cout << "Tile drawing!!!" << endl;
                return;
            }

            // Save current pc value in stack
            _registers.dec_sp();
            _memory.write(_registers.get_sp(), msb(_registers.get_pc()));
            _registers.dec_sp();
            _memory.write(_registers.get_sp(), lsb(_registers.get_pc()));

            _registers.set_pc(next_u16);

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
        case 0xD1:
            cout << "POP DE" << endl;

            _registers.set_de(_pop_stack());

            break;
        case 0xD2:
            cout << "JP NC,u16" << endl;

            next_u16 = _read16(rom);

            if (_registers.get_flag_c() == 0) {
                _registers.set_pc(next_u16);
            }

            break;
        case 0xD5:
            cout << "PUSH DE" << endl;

            _push_stack(_registers.get_de());

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
        case 0xDA:
            cout << "JP C,u16" << endl;

            next_u16 = _read16(rom);

            if (_registers.get_flag_c() == 1) {
                _registers.set_pc(next_u16);
            }

            break;
        case 0xE0:
            cout << "LD (FF00+u8),A" << endl;

            _memory.write(0xFF00 + _read8(rom), _registers.get_a());

            break;
        case 0xE1:
            cout << "POP HL" << endl;

            _registers.set_hl(_pop_stack());

            break;
        case 0xE5:
            cout << "PUSH HL" << endl;

            _push_stack(_registers.get_hl());

            break;
        case 0xE9:
            cout << "JP HL" << endl;

            _registers.set_pc(_registers.get_hl());

            break;
        case 0xEA:
            cout << "LD (u16),A" << endl;

            next_u16 = _read16(rom);
            _memory.write(next_u16, _registers.get_a());

            break;
        case 0xEE:
        {
            cout << "XOR A,u8" << endl;

            u8 res = _registers.get_a() ^ _read8(rom);
            _registers.set_a(res);

            _registers.set_flag_z(res == 0);
            _registers.set_flag_z(0);
            _registers.set_flag_h(0);
            _registers.set_flag_c(0);

            break;
        }
        case 0xF0:
            cout << "LD A,(FF00+u8)" << endl;

            next_u8 = _read8(rom);
            _registers.set_a(_memory.read(get_u16(next_u8, 0xFF)));

            break;
        case 0xF1:
            cout << "POP AF" << endl;

            _registers.set_af(_pop_stack());

            break;
        case 0xF3:
            cout << "DI" << endl;
            // TODO
            break;
        case 0xFD:
            cout << "Undefined" << endl;

            return;

            break;
        case 0xF5:
            cout << "PUSH AF" << endl;
            
            _push_stack(_registers.get_af());

            break;
        case 0xFA:
            cout << "LD A,(u16)" << endl;

            next_u16 = _read16(rom);
            _registers.set_a(_memory.read(next_u16));

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
            //case 0x19:
            //    cout << "RR C" << endl;

            //    //comeback;
            //    //refer to page 101 of manual to implement rlc, rl, rrc, rr
            //    //maybe use a smaller test rom
            case 0x38:
                cout << "SRL B" << endl;

                _registers.set_b(_srl(_registers.get_b()));

                break;
            case 0x39:
                cout << "SRL C" << endl;

                _registers.set_c(_srl(_registers.get_c()));

                break;
            case 0x3A:
                cout << "SRL D" << endl;

                _registers.set_d(_srl(_registers.get_d()));

                break;
            case 0x3B:
                cout << "SRL E" << endl;

                _registers.set_e(_srl(_registers.get_e()));

                break;
            case 0x3C:
                cout << "SRL H" << endl;

                _registers.set_h(_srl(_registers.get_h()));

                break;
            case 0x3D:
                cout << "SRL L" << endl;

                _registers.set_l(_srl(_registers.get_l()));

                break;
            case 0x3E:
            {
                cout << "SRL (HL)" << endl;

                u16 hl_addr = _registers.get_hl();

                _memory.write(hl_addr, _srl(_memory.read(hl_addr)));

                break;
            }
            case 0x3F:
                cout << "SRL A" << endl;

                _registers.set_a(_srl(_registers.get_a()));

                break;
            case 0x7E:
                cout << "BIT 7,(HL)" << endl;

                _registers.set_flag_z(get_bit(_memory.read(_registers.get_hl()), 7) == 0);
                _registers.set_flag_n(0);
                _registers.set_flag_h(1);

                break;
            default:
                cout << "Unrecognized (extended) opcode" << endl;
                return;
            }

            break;
        }
        default:
            cout << "Unrecognized opcode" << endl;
            return;
        }

        //if (counter++ == 50) {
        //    break;
        //}
    }

    printf("pc: %X ", _registers.get_pc());
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

void Cpu::_push_stack(const u16 val) {
    const u8 msb = val >> 8;
    const u8 lsb = val & 0xFF;

    _registers.dec_sp();
    _memory.write(_registers.get_sp(), msb);
    _registers.dec_sp();
    _memory.write(_registers.get_sp(), lsb);
}

u16 Cpu::_pop_stack() {
    const u8 lsb = _memory.read(_registers.get_sp());
    _registers.inc_sp();
    const u8 msb = _memory.read(_registers.get_sp());
    _registers.inc_sp();

    return get_u16(lsb, msb);
}


void Cpu::_cp(const u8 val1, const u8 val2) {
    i8 res = val1 - val2;

    _registers.set_flag_z(res == 0);
    _registers.set_flag_n(1);
    _registers.set_flag_h((i8)((val1 & 0xF) - (val2 & 0xF)) < 0);
    _registers.set_flag_c(res < 0);
}

// For INC n - incrementing register n.
// Increments the given value and returns it, and 
// sets flags appropriately.
u8 Cpu::_inc(const u8 val) {
    u8 res = val + 1;

    _registers.set_flag_z(res == 0);
    _registers.set_flag_n(0);
    _registers.set_flag_h((val & 0xF) == 0xF);

    return res;
}

// Performs arithmetic shift right to the given val (into carry flag), 
// and returns the result. 
// Also sets flags appropriately.
u8 Cpu::_sra(const u8 val) {
    _registers.set_flag_c(val & 1);

    u8 res = (val & 0b10000000) | (val >> 1);
    _registers.set_flag_z(res == 0);

    _registers.set_flag_n(0);
    _registers.set_flag_h(0);

    return res;
}

// Performs logical shift right to the given val (into carry flag), 
// and returns the result. 
// Also sets flags appropriately.
u8 Cpu::_srl(const u8 val) {
    _registers.set_flag_c(val & 1);

    u8 res = val >> 1;
    _registers.set_flag_z(res == 0);

    _registers.set_flag_n(0);
    _registers.set_flag_h(0);

    return res;
}