#include <iostream>

#include "Cpu.h"

#include "utils.h"


using namespace std;


Cpu::Cpu(Memory& memory) : _memory(memory), _registers(Registers{}) {
    is_on = true;

    _ime = true;

    // Power up (reset register values)
    // Initialize program counter and stack pointer 
    _registers.set_pc(0x100);
    _registers.set_sp(0xFFFE);

    _registers.set_af(0x01B0);
    _registers.set_bc(0x0013);
    _registers.set_de(0x00D8);
    _registers.set_hl(0x014D);

    _memory.write(0xFF05, 0x00);    // TIMA 
    _memory.write(0xFF06, 0x00);    // TMA
    _memory.write(0xFF07, 0x00);    // TAC
    _memory.write(0xFF10, 0x80);    // NR10
    _memory.write(0xFF11, 0xBF);    // NR11
    _memory.write(0xFF12, 0xF3);    // NR12
    _memory.write(0xFF14, 0xBF);    // NR14
    _memory.write(0xFF16, 0x3F);    // NR21
    _memory.write(0xFF17, 0x00);    // NR22
    _memory.write(0xFF19, 0xBF);    // NR24
    _memory.write(0xFF1A, 0x7F);    // NR30
    _memory.write(0xFF1B, 0xFF);    // NR31
    _memory.write(0xFF1C, 0x9F);    // NR32
    _memory.write(0xFF1E, 0xBF);    // NR34
    _memory.write(0xFF20, 0xFF);    // NR41
    _memory.write(0xFF21, 0x00);    // NR42
    _memory.write(0xFF22, 0x00);    // NR43
    _memory.write(0xFF23, 0xBF);    // NR44
    _memory.write(0xFF24, 0x77);    // NR50
    _memory.write(0xFF25, 0xF3);    // NR51
    _memory.write(0xFF26, 0xF1);    // NR52
    _memory.write(0xFF40, 0x91);    // LCDC
    _memory.write(0xFF42, 0x00);    // SCY
    _memory.write(0xFF43, 0x00);    // SCX
    _memory.write(0xFF45, 0x00);    // LYC
    _memory.write(0xFF47, 0xFC);    // BGP
    _memory.write(0xFF48, 0xFF);    // 0BP0
    _memory.write(0xFF49, 0xFF);    // 0BP1
    _memory.write(0xFF4A, 0x00);    // WY
    _memory.write(0xFF4B, 0x00);    // WX
    _memory.write(0xFFFF, 0x00);    // IE

    // TODO: Remove this later
    // Disable boot ROM
    //_memory.write(0xFF50, 0x01);

    // TODO: Remove this after implementing cycle and LCD properly
    // This is only maually set here so that we don't get stuck in a loop
    // while waiting for the value of LY to change.
    //_memory.write(0xFF44, 0x90);
    //_memory.write(0xFF44, 0x91);
}

int tile_count = 0;
int line_count = 0;

// Execute the next program instruction with the current pc and returns the
// number of cycles taken to execute the instruction
unsigned int Cpu::execute_next() {
    u8 opcode = _memory.read(_registers.get_pc());

    // printf("pc: %X ", _registers.get_pc());

    if (_registers.get_pc() == 0x01FD) {
        //cout << "vblank interrupt handler here. check values" << endl;
    }

    _registers.inc_pc();

    // printf("%X ", opcode);

    u8 next_u8;
    u16 next_u16;

    switch (opcode) {
    case 0x00:
        ////cout << "NOP" << endl;
        break;
    case 0x01:
        //cout << "LD BC,u16" << endl;

        _registers.set_bc(_read16());

        return 12;
    case 0x02:
        //cout << "LD (BC),A" << endl;

        _memory.write(_registers.get_bc(), _registers.get_a());

        return 8;
    case 0x03:
        //cout << "INC BC" << endl;

        _registers.inc_bc();

        return 8;
    case 0x04:
        //cout << "INC B" << endl;

        _registers.set_b(_inc(_registers.get_b()));

        break;
    case 0x05:
    {
        //cout << "DEC B" << endl;

        u8 res = _registers.get_b() - 1;
        _registers.set_b(res);

        _registers.set_flag_z(res == 0);
        _registers.set_flag_n(1);
        _registers.set_flag_h((res & 0x0F) == 0x0F);

        break;
    }
    case 0x06:
        //cout << "LD B,u8" << endl;

        _registers.set_b(_read8());

        return 8;
    case 0x07:
        //cout << "RLCA" << endl;

        _registers.set_a(_rlc(_registers.get_a()));

        break;
    case 0x08:
    {
        //cout << "LD (u16),SP" << endl;

        next_u16 = _read16();

        u16 sp_val = _registers.get_sp();
        _memory.write(next_u16, lsb(sp_val));
        _memory.write(next_u16 + 1, msb(sp_val));

        return 20;
    }
    case 0x09:
        //cout << "ADD HL,BC" << endl;

        _add_hl(_registers.get_bc());

        return 8;
    case 0x0A:
        //cout << "LD A,(BC)" << endl;

        _registers.set_a(_memory.read(_registers.get_bc()));

        return 8;
    case 0x0B:
        //cout << "DEC BC" << endl;

        _registers.dec_bc();

        return 8;
    case 0x0C:
        //cout << "INC C" << endl;

        _registers.set_c(_inc(_registers.get_c()));

        break;
    case 0x0D:
    {
        //cout << "DEC C" << endl;

        u8 res = _registers.get_c() - 1;
        _registers.set_c(res);

        _registers.set_flag_z(res == 0);
        _registers.set_flag_n(1);
        _registers.set_flag_h((res & 0x0F) == 0x0F);

        break;
    }
    case 0x0E:
        //cout << "LD C,u8" << endl;

        _registers.set_c(_read8());

        return 8;
    case 0x11:
        //cout << "LD DE,u16" << endl;

        _registers.set_de(_read16());

        return 12;
    case 0x12:
        //cout << "LD (DE),A" << endl;

        _memory.write(_registers.get_de(), _registers.get_a());

        return 8;
    case 0x13:
        //cout << "INC DE" << endl;

        _registers.inc_de();

        return 8;
    case 0x14:
        //cout << "INC D" << endl;

        _registers.set_d(_inc(_registers.get_d()));

        break;
    case 0x15:
    {
        //cout << "DEC D" << endl;

        u8 res = _registers.get_d() - 1;
        _registers.set_d(res);

        _registers.set_flag_z(res == 0);
        _registers.set_flag_n(1);
        _registers.set_flag_h((res & 0x0F) == 0x0F);

        break;
    }
    case 0x16:
        //cout << "LD D,u8" << endl;

        _registers.set_d(_read8());

        return 8;
    case 0x18:
        //cout << "JR i8" << endl;

        next_u8 = _read8();
        _registers.set_pc(_registers.get_pc() + (i8)next_u8);

        return 12;
    case 0x19:
        //cout << "ADD HL,DE" << endl;

        _add_hl(_registers.get_de());

        return 8;
    case 0x1A:
        //cout << "LD A,(DE)" << endl;

        _registers.set_a(_memory.read(_registers.get_de()));

        return 8;
    case 0x1B:
        //cout << "DEC DE" << endl;

        _registers.dec_de();

        return 8;
    case 0x1C:
        //cout << "INC E" << endl;

        _registers.set_e(_inc(_registers.get_e()));

        break;
    case 0x1D:
    {
        //cout << "DEC E" << endl;

        u8 res = _registers.get_e() - 1;
        _registers.set_e(res);

        _registers.set_flag_z(res == 0);
        _registers.set_flag_n(1);
        _registers.set_flag_h((res & 0x0F) == 0x0F);

        break;
    }
    case 0x1E:
        //cout << "LD E,u8" << endl;

        _registers.set_e(_read8());

        return 8;
    case 0x1F:
        //cout << "RRA" << endl;

        // TODO: May have to set clear flag_z - conflicting manuals
        _registers.set_a(_rr(_registers.get_a()));

        break;
    case 0x20:
        //cout << "JR NZ,i8" << endl;

        next_u8 = _read8();

        if (_registers.get_flag_z() == 0) {
            _registers.set_pc(_registers.get_pc() + (i8)next_u8);
            
            return 12;
        }

        return 8;
    case 0x21:
        //cout << "LD HL,u16" << endl;

        _registers.set_hl(_read16());

        return 12;
    case 0x22:
        //cout << "LD (HL+),A" << endl;

        _memory.write(_registers.get_hl(), _registers.get_a());
        _registers.inc_hl();

        return 8;
    case 0x23:
        //cout << "INC HL" << endl;

        _registers.inc_hl();

        return 8;
    case 0x24:
        //cout << "INC H" << endl;

        _registers.set_h(_inc(_registers.get_h()));

        break;
    case 0x25:
    {
        //cout << "DEC H" << endl;

        u8 res = _registers.get_h() - 1;
        _registers.set_h(res);

        _registers.set_flag_z(res == 0);
        _registers.set_flag_n(1);
        _registers.set_flag_h((res & 0x0F) == 0x0F);

        break;
    }
    case 0x26:
        //cout << "LD H,u8" << endl;

        _registers.set_h(_read8());

        return 8;
    case 0x27:
    {
        //cout << "DAA" << endl;

        bool flag_c = _registers.get_flag_c();
        bool flag_h = _registers.get_flag_h();

        // Case in which the previous arithmetic instruction was an addition
        if (_registers.get_flag_n() == 0) {
            // Adjust if half carry or carry occurred or if result is out of bounds

            if (flag_c == 1 || _registers.get_a() > 0x99) {
                _registers.set_a(_registers.get_a() + 0x60);
                _registers.set_flag_c(1);
            }
            
            if (flag_h == 1 || (_registers.get_a() & 0x0F) > 0x09) {
                _registers.set_a(_registers.get_a() + 0x6);
            }
        }
        // Case in which the previous arithmetic instruction was a subtraction
        else {
            // Adjust if half carry or carry occurred

            if (flag_c == 1) {
                _registers.set_a(_registers.get_a() - 0x60);
            }

            if (flag_h == 1) {
                _registers.set_a(_registers.get_a() - 0x6);
            }
        }

        _registers.set_flag_z(_registers.get_a() == 0);
        _registers.set_flag_h(0);

        break;
    }
    case 0x28:
        //cout << "JR Z,i8" << endl;

        next_u8 = _read8();

        if (_registers.get_flag_z() == 1) {
            _registers.set_pc(_registers.get_pc() + (i8)next_u8);

            return 12;
        }

        return 8;
    case 0x29:
        //cout << "ADD HL,HL" << endl;

        _add_hl(_registers.get_hl());

        return 8;
    case 0x2A:
        //cout << "LD A,(HL+)" << endl;

        _registers.set_a(_memory.read(_registers.get_hl()));
        _registers.inc_hl();

        return 8;
    case 0x2B:
        //cout << "DEC HL" << endl;

        _registers.dec_hl();

        return 8;
    case 0x2C:
        //cout << "INC L" << endl;

        _registers.set_l(_inc(_registers.get_l()));

        break;
    case 0x2D:
    {
        //cout << "DEC L" << endl;

        u8 res = _registers.get_l() - 1;
        _registers.set_l(res);

        _registers.set_flag_z(res == 0);
        _registers.set_flag_n(1);
        _registers.set_flag_h((res & 0x0F) == 0x0F);

        break;
    }
    case 0x2E:
        //cout << "LD L,u8" << endl;

        _registers.set_l(_read8());

        return 8;
    case 0x2F:
        //cout << "CPL" << endl;

        _registers.set_a(~_registers.get_a());
        _registers.set_flag_n(1);
        _registers.set_flag_h(1);

        break;
    case 0x30:
        //cout << "JR NC,i8" << endl;

        next_u8 = _read8();

        if (_registers.get_flag_c() == 0) {
            _registers.set_pc(_registers.get_pc() + (i8)next_u8);

            return 12;
        }

        return 8;
    case 0x31:
        //cout << "LD SP,u16" << endl;

        _registers.set_sp(_read16());

        return 12;
    case 0x32:
        //cout << "LD (HL-),A" << endl;
        
        _memory.write(_registers.get_hl(), _registers.get_a());
        _registers.dec_hl();

        return 8;
    case 0x33:
        //cout << "INC SP" << endl;

        _registers.inc_sp();

        return 8;
    case 0x34:
    {
        //cout << "INC (HL)" << endl;

        u16 hl_addr = _registers.get_hl();
        _memory.write(hl_addr, _inc(_memory.read(hl_addr)));

        return 12;
    }
    case 0x35:
    {
        //cout << "DEC (HL)" << endl;

        u8 res = _memory.read(_registers.get_hl()) - 1;
        _memory.write(_registers.get_hl(), res);

        _registers.set_flag_z(res == 0);
        _registers.set_flag_n(1);
        _registers.set_flag_h((res & 0x0F) == 0x0F);

        return 12;
    }
    case 0x36:
        //cout << "LD (HL),u8" << endl;

        _memory.write(_registers.get_hl(), _read8());

        return 12;
    case 0x38:
        //cout << "JR C,i8" << endl;

        next_u8 = _read8();

        if (_registers.get_flag_c() == 1) {
            _registers.set_pc(_registers.get_pc() + (i8)next_u8);

            return 12;
        }

        return 8;
    case 0x39:
        //cout << "ADD HL,SP" << endl;

        _add_hl(_registers.get_sp());

        return 8;
    case 0x3A:
        //cout << "LD A,(HL-)" << endl;

        _registers.set_a(_memory.read(_registers.get_hl()));
        _registers.dec_hl();

        return 8;
    case 0x3B:
        //cout << "DEC SP" << endl;

        _registers.dec_sp();

        return 8;
    case 0x3C:
        //cout << "INC A" << endl;

        _registers.set_a(_inc(_registers.get_a()));

        return 8;
    case 0x3D:
    {
        //cout << "DEC A" << endl;

        u8 res = _registers.get_a() - 1;
        _registers.set_a(res);

        _registers.set_flag_z(res == 0);
        _registers.set_flag_n(1);
        _registers.set_flag_h((res & 0x0F) == 0x0F);

        break;
    }
    case 0x3E:
        //cout << "LD A,u8" << endl;

        next_u8 = _read8();
        _registers.set_a(next_u8);

        return 8;
    case 0x40:
        //cout << "LD B,B" << endl;

        // NOP

        break;
    case 0x41:
        //cout << "LD B,C" << endl;

        _registers.set_b(_registers.get_c());

        break;
    case 0x42:
        //cout << "LD B,D" << endl;

        _registers.set_b(_registers.get_d());

        break;
    case 0x43:
        //cout << "LD B,E" << endl;

        _registers.set_b(_registers.get_e());

        break;
    case 0x44:
        //cout << "LD B,H" << endl;

        _registers.set_b(_registers.get_h());

        break;
    case 0x45:
        //cout << "LD B,L" << endl;

        _registers.set_b(_registers.get_l());

        break;
    case 0x46:
        //cout << "LD B,(HL)" << endl;

        _registers.set_b(_memory.read(_registers.get_hl()));

        return 8;
    case 0x47:
        //cout << "LD B,A" << endl;

        _registers.set_b(_registers.get_a());

        break;
    case 0x48:
        //cout << "LD C,B" << endl;

        _registers.set_c(_registers.get_b());

        break;
    case 0x49:
        //cout << "LD C,C" << endl;

        // NOP

        break;
    case 0x4A:
        //cout << "LD C,D" << endl;

        _registers.set_c(_registers.get_d());

        break;
    case 0x4B:
        //cout << "LD C,E" << endl;

        _registers.set_c(_registers.get_e());

        break;
    case 0x4C:
        //cout << "LD C,H" << endl;

        _registers.set_c(_registers.get_h());

        break;
    case 0x4D:
        //cout << "LD C,L" << endl;

        _registers.set_c(_registers.get_l());

        break;
    case 0x4E:
        //cout << "LD C,(HL)" << endl;

        _registers.set_c(_memory.read(_registers.get_hl()));

        return 8;
    case 0x4F:
        //cout << "LD C,A" << endl;

        _registers.set_c(_registers.get_a());

        break;
    case 0x50:
        //cout << "LD D,B" << endl;

        _registers.set_d(_registers.get_b());

        break;
    case 0x51:
        //cout << "LD D,C" << endl;

        _registers.set_d(_registers.get_c());

        break;
    case 0x52:
        //cout << "LD D,D" << endl;

        // NOP

        break;
    case 0x53:
        //cout << "LD D,E" << endl;

        _registers.set_d(_registers.get_e());

        break;
    case 0x54:
        //cout << "LD D,H" << endl;

        _registers.set_d(_registers.get_h());

        break;
    case 0x55:
        //cout << "LD D,L" << endl;

        _registers.set_d(_registers.get_l());

        break;
    case 0x56:
        //cout << "LD D,(HL)" << endl;

        _registers.set_d(_memory.read(_registers.get_hl()));

        return 8;
    case 0x57:
        //cout << "LD D,A" << endl;

        _registers.set_d(_registers.get_a());

        break;
    case 0x58:
        //cout << "LD E,B" << endl;

        _registers.set_e(_registers.get_b());

        break;
    case 0x59:
        //cout << "LD E,C" << endl;

        _registers.set_e(_registers.get_c());

        break;
    case 0x5A:
        //cout << "LD E,D" << endl;

        _registers.set_e(_registers.get_d());

        break;
    case 0x5B:
        //cout << "LD E,E" << endl;

        // NOP

        break;
    case 0x5C:
        //cout << "LD E,H" << endl;

        _registers.set_e(_registers.get_h());

        break;
    case 0x5D:
        //cout << "LD E,L" << endl;

        _registers.set_e(_registers.get_l());

        break;
    case 0x5E:
        //cout << "LD E,(HL)" << endl;

        _registers.set_e(_memory.read(_registers.get_hl()));

        return 8;
    case 0x5F:
        //cout << "LD E,A" << endl;

        _registers.set_e(_registers.get_a());

        break;
    case 0x60:
        //cout << "LD H,B" << endl;

        _registers.set_h(_registers.get_b());

        break;
    case 0x61:
        //cout << "LD H,C" << endl;

        _registers.set_h(_registers.get_c());

        break;
    case 0x62:
        //cout << "LD H,D" << endl;

        _registers.set_h(_registers.get_d());

        break;
    case 0x63:
        //cout << "LD H,E" << endl;

        _registers.set_h(_registers.get_e());

        break;
    case 0x64:
        //cout << "LD H,H" << endl;

        // NOP

        break;
    case 0x65:
        //cout << "LD H,L" << endl;

        _registers.set_h(_registers.get_l());

        break;
    case 0x66:
        //cout << "LD H,(HL)" << endl;

        _registers.set_h(_memory.read(_registers.get_hl()));

        return 8;
    case 0x67:
        //cout << "LD H,A" << endl;

        _registers.set_h(_registers.get_a());

        break;
    case 0x68:
        //cout << "LD L,B" << endl;

        _registers.set_l(_registers.get_b());

        break;
    case 0x69:
        //cout << "LD L,C" << endl;

        _registers.set_l(_registers.get_c());

        break;
    case 0x6A:
        //cout << "LD L,D" << endl;

        _registers.set_l(_registers.get_d());

        break;
    case 0x6B:
        //cout << "LD L,E" << endl;

        _registers.set_l(_registers.get_e());

        break;
    case 0x6C:
        //cout << "LD L,H" << endl;

        _registers.set_l(_registers.get_h());

        break;
    case 0x6D:
        //cout << "LD L,L" << endl;

        // NOP

        break;
    case 0x6E:
        //cout << "LD L,(HL)" << endl;

        _registers.set_l(_memory.read(_registers.get_hl()));

        return 8;
    case 0x6F:
        //cout << "LD L,A" << endl;

        _registers.set_l(_registers.get_a());

        break;
    case 0x70:
        //cout << "LD (HL),B" << endl;

        _memory.write(_registers.get_hl(), _registers.get_b());

        return 8;
    case 0x71:
        //cout << "LD (HL),C" << endl;

        _memory.write(_registers.get_hl(), _registers.get_c());

        return 8;
    case 0x72:
        //cout << "LD (HL),D" << endl;

        _memory.write(_registers.get_hl(), _registers.get_d());

        return 8;
    case 0x73:
        //cout << "LD (HL),E" << endl;

        _memory.write(_registers.get_hl(), _registers.get_e());

        return 8;
    case 0x74:
        //cout << "LD (HL),H" << endl;

        _memory.write(_registers.get_hl(), _registers.get_h());

        return 8;
    case 0x75:
        //cout << "LD (HL),L" << endl;

        _memory.write(_registers.get_hl(), _registers.get_l());

        return 8;
    case 0x76:
        //cout << "HALT" << endl;

        is_on = false;

        return 4;
    case 0x77:
        //cout << "LD (HL),A" << endl;

        _memory.write(_registers.get_hl(), _registers.get_a());

        return 8;
    case 0x78:
        //cout << "LD A,B" << endl;

        _registers.set_a(_registers.get_b());

        break;
    case 0x79:
        //cout << "LD A,C" << endl;

        _registers.set_a(_registers.get_c());

        break;
    case 0x7A:
        //cout << "LD A,D" << endl;

        _registers.set_a(_registers.get_d());

        break;
    case 0x7B:
        //cout << "LD A,E" << endl;

        _registers.set_a(_registers.get_e());

        break;
    case 0x7C:
        //cout << "LD A,H" << endl;

        _registers.set_a(_registers.get_h());

        break;
    case 0x7D:
        //cout << "LD A,L" << endl;

        _registers.set_a(_registers.get_l());

        break;
    case 0x7E:
        //cout << "LD A,(HL)" << endl;

        _registers.set_a(_memory.read(_registers.get_hl()));

        return 8;
    case 0x7F:
        //cout << "LD A,A" << endl;

        // NOP

        break;
    case 0x80:
        //cout << "ADD A,B" << endl;

        _registers.set_a(_add(_registers.get_a(), _registers.get_b()));

        break;
    case 0x81:
        //cout << "ADD A,C" << endl;

        _registers.set_a(_add(_registers.get_a(), _registers.get_c()));

        break;
    case 0x82:
        //cout << "ADD A,D" << endl;

        _registers.set_a(_add(_registers.get_a(), _registers.get_d()));

        break;
    case 0x83:
        //cout << "ADD A,E" << endl;

        _registers.set_a(_add(_registers.get_a(), _registers.get_e()));

        break;
    case 0x84:
        //cout << "ADD A,H" << endl;

        _registers.set_a(_add(_registers.get_a(), _registers.get_h()));

        break;
    case 0x85:
        //cout << "ADD A,L" << endl;

        _registers.set_a(_add(_registers.get_a(), _registers.get_l()));

        break;
    case 0x86:
        //cout << "ADD A,(HL)" << endl;

        _registers.set_a(_add(_registers.get_a(), _memory.read(_registers.get_hl())));

        return 8;
    case 0x87:
        //cout << "ADD A,A" << endl;

        _registers.set_a(_add(_registers.get_a(), _registers.get_a()));

        break;
    case 0x88:
        //cout << "ADC A,B" << endl;

        _adc_a(_registers.get_b());

        break;
    case 0x89:
        //cout << "ADC A,C" << endl;

        _adc_a(_registers.get_c());

        break;
    case 0x8A:
        //cout << "ADC A,D" << endl;

        _adc_a(_registers.get_d());

        break;
    case 0x8B:
        //cout << "ADC A,E" << endl;

        _adc_a(_registers.get_e());

        break;
    case 0x8C:
        //cout << "ADC A,H" << endl;

        _adc_a(_registers.get_h());

        break;
    case 0x8D:
        //cout << "ADC A,L" << endl;

        _adc_a(_registers.get_l());

        break;
    case 0x8E:
        //cout << "ADC A,(HL)" << endl;

        _adc_a(_memory.read(_registers.get_hl()));

        return 8;
    case 0x8F:
        //cout << "ADC A,A" << endl;

        _adc_a(_registers.get_a());

        break;
    case 0xA0:
        //cout << "AND A,B" << endl;

        _registers.set_a(_and(_registers.get_a(), _registers.get_b()));

        break;
    case 0xA1:
        //cout << "AND A,C" << endl;

        _registers.set_a(_and(_registers.get_a(), _registers.get_c()));

        break;
    case 0xA2:
        //cout << "AND A,D" << endl;

        _registers.set_a(_and(_registers.get_a(), _registers.get_d()));

        break;
    case 0xA3:
        //cout << "AND A,E" << endl;

        _registers.set_a(_and(_registers.get_a(), _registers.get_e()));

        break;
    case 0xA4:
        //cout << "AND A,H" << endl;

        _registers.set_a(_and(_registers.get_a(), _registers.get_h()));

        break;
    case 0xA5:
        //cout << "AND A,L" << endl;

        _registers.set_a(_and(_registers.get_a(), _registers.get_l()));

        break;
    case 0xA6:
        //cout << "AND A,(HL)" << endl;

        _registers.set_a(_and(_registers.get_a(), _memory.read(_registers.get_hl())));

        return 8;
    case 0xA7:
        //cout << "AND A,A" << endl;

        _registers.set_a(_and(_registers.get_a(), _registers.get_a()));

        break;
    case 0xA8:
    {
        //cout << "XOR A,B" << endl;

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
        //cout << "XOR A,C" << endl;

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
        //cout << "XOR A,D" << endl;

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
        //cout << "XOR A,E" << endl;

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
        //cout << "XOR A,H" << endl;

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
        //cout << "XOR A,L" << endl;

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
        //cout << "XOR A,(HL)" << endl;

        u8 res = _registers.get_a() ^ _memory.read(_registers.get_hl());
        _registers.set_a(res);
            
        _registers.set_flag_z(res == 0);
        _registers.set_flag_z(0);
        _registers.set_flag_h(0);
        _registers.set_flag_c(0);

        return 8;
    }
    case 0xAF:
        //cout << "XOR A,A" << endl;

        _registers.set_a(0);

        _registers.set_flag_z(1);
        _registers.set_flag_z(0);
        _registers.set_flag_h(0);
        _registers.set_flag_c(0);

        break;
    case 0xB0:
        //cout << "OR A,B" << endl;

        _registers.set_a(_or(_registers.get_a(), _registers.get_b()));

        break;
    case 0xB1:
        //cout << "OR A,C" << endl;
            
        _registers.set_a(_or(_registers.get_a(), _registers.get_c()));

        break;
    case 0xB2:
        //cout << "OR A,D" << endl;

        _registers.set_a(_or(_registers.get_a(), _registers.get_d()));

        break;
    case 0xB3:
        //cout << "OR A,E" << endl;

        _registers.set_a(_or(_registers.get_a(), _registers.get_e()));

        break;
    case 0xB4:
        //cout << "OR A,H" << endl;

        _registers.set_a(_or(_registers.get_a(), _registers.get_h()));

        break;
    case 0xB5:
        //cout << "OR A,L" << endl;

        _registers.set_a(_or(_registers.get_a(), _registers.get_l()));

        break;
    case 0xB6:
        //cout << "OR A,(HL)" << endl;

        _registers.set_a(_or(_registers.get_a(), _memory.read(_registers.get_hl())));

        return 8;
    case 0xB7:
        //cout << "OR A,A" << endl;

        // TODO: Could optimize
        _registers.set_a(_or(_registers.get_a(), _registers.get_a()));

        break;
    case 0xB8:
        //cout << "CP A,B" << endl;

        _cp(_registers.get_a(), _registers.get_b());

        break;
    case 0xB9:
        //cout << "CP A,C" << endl;

        _cp(_registers.get_a(), _registers.get_c());

        break;
    case 0xBA:
        //cout << "CP A,D" << endl;

        _cp(_registers.get_a(), _registers.get_d());

        break;
    case 0xBB:
        //cout << "CP A,E" << endl;

        _cp(_registers.get_a(), _registers.get_e());

        break;
    case 0xBC:
        //cout << "CP A,H" << endl;

        _cp(_registers.get_a(), _registers.get_h());

        break;
    case 0xBD:
        //cout << "CP A,L" << endl;

        _cp(_registers.get_a(), _registers.get_l());

        break;
    case 0xBE:
        //cout << "CP A,(HL)" << endl;

        _cp(_registers.get_a(), _memory.read(_registers.get_hl()));

        return 8;
    case 0xBF:
        //cout << "CP A,A" << endl;

        _cp(_registers.get_a(), _registers.get_a());

        break;
    case 0xC0:
        //cout << "RET NZ" << endl;

        if (_registers.get_flag_z() == 0) {
            u8 lsb = _memory.read(_registers.get_sp());
            _registers.inc_sp();
            u8 msb = _memory.read(_registers.get_sp());
            _registers.inc_sp();
            _registers.set_pc(get_u16(lsb, msb));

            return 20;
        }

        return 8;
    case 0xC1:
        //cout << "POP BC" << endl;
            
        _registers.set_bc(_pop_stack());

        return 12;
    case 0xC2:
        //cout << "JP NZ,u16" << endl;

        next_u16 = _read16();

        if (_registers.get_flag_z() == 0) {
            _registers.set_pc(next_u16);

            return 16;
        }

        return 12;
    case 0xC3:
        //cout << "JP u16" << endl;

        _registers.set_pc(_read16());
        ////cout << "Set PC to " << u16_to_binary(_registers.get_pc()) << endl;

        return 16;
    case 0xC4:
        //cout << "CALL NZ,u16" << endl;

        next_u16 = _read16();

        if (_registers.get_flag_z() == 0) {
            _push_stack(_registers.get_pc());
            _registers.set_pc(next_u16);

            return 24;
        }

        return 12;
    case 0xC5:
        //cout << "PUSH BC" << endl;

        _push_stack(_registers.get_bc());

        return 16;
    case 0xC6:
        //cout << "ADD A,u8" << endl;
        
        _registers.set_a(_add(_registers.get_a(), _read8()));

        return 8;
    case 0xC7:
        //cout << "RST 00h" << endl;

        _rst(0x00);

        return 16;
    case 0xC8:
        //cout << "RET Z" << endl;

        if (_registers.get_flag_z() == 1) {
            u8 lsb = _memory.read(_registers.get_sp());
            _registers.inc_sp();
            u8 msb = _memory.read(_registers.get_sp());
            _registers.inc_sp();
            _registers.set_pc(get_u16(lsb, msb));

            return 20;
        }

        return 8;
    case 0xC9:
    {
        //cout << "RET" << endl;

        _registers.set_pc(_pop_stack());

        return 16;
    }
    case 0xCA:
        //cout << "JP Z,u16" << endl;

        next_u16 = _read16();

        if (_registers.get_flag_z() == 1) {
            _registers.set_pc(next_u16);

            return 16;
        }

        return 12;
    case 0xCD:
        //cout << "CALL u16" << endl;

        next_u16 = _read16();
                
        //// printf("Called function: %X\n", next_u16);

        // Save current pc value in stack
        _push_stack(_registers.get_pc());

        _registers.set_pc(next_u16);

        return 24;
    case 0xCE:
        //cout << "ADC A,u8" << endl;

        _adc_a(_read8());

        return 8;
    case 0xCF:
        //cout << "RST 08h" << endl;

        _rst(0x08);

        return 16;
    case 0xD0:
        //cout << "RET NC" << endl;

        if (_registers.get_flag_c() == 0) {
            u8 lsb = _memory.read(_registers.get_sp());
            _registers.inc_sp();
            u8 msb = _memory.read(_registers.get_sp());
            _registers.inc_sp();
            _registers.set_pc(get_u16(lsb, msb));
            
            return 20;
        }

        return 8;
    case 0xD1:
        //cout << "POP DE" << endl;

        _registers.set_de(_pop_stack());

        return 12;
    case 0xD2:
        //cout << "JP NC,u16" << endl;

        next_u16 = _read16();

        if (_registers.get_flag_c() == 0) {
            _registers.set_pc(next_u16);

            return 16;
        }

        return 12;
    case 0xD5:
        //cout << "PUSH DE" << endl;

        _push_stack(_registers.get_de());

        return 16;
    case 0xD6:
        //cout << "SUB A,u8" << endl;
        
        _registers.set_a(_sub(_registers.get_a(), _read8()));

        return 8;
    case 0xD7:
        //cout << "RST 10h" << endl;

        _rst(0x10);

        return 16;
    case 0xD8:
        //cout << "RET C" << endl;

        if (_registers.get_flag_c() == 1) {
            u8 lsb = _memory.read(_registers.get_sp());
            _registers.inc_sp();
            u8 msb = _memory.read(_registers.get_sp());
            _registers.inc_sp();
            _registers.set_pc(get_u16(lsb, msb));

            return 20;
        }

        return 8;
    case 0xD9:
        //cout << "RETI" << endl;

        _registers.set_pc(_pop_stack());
        _ime = 1;

        return 16;
    case 0xDA:
        //cout << "JP C,u16" << endl;

        next_u16 = _read16();

        if (_registers.get_flag_c() == 1) {
            _registers.set_pc(next_u16);

            return 16;
        }

        return 12;
    case 0xDF:
        //cout << "RST 18h" << endl;

        _rst(0x18);

        return 16;
    case 0xE0:
        //cout << "LD (FF00+u8),A" << endl;

        _memory.write(0xFF00 + _read8(), _registers.get_a());

        return 12;
    case 0xE1:
        //cout << "POP HL" << endl;

        _registers.set_hl(_pop_stack());

        return 12;
    case 0xE2:
        //cout << "LD (FF00+C),A" << endl;

        _memory.write(0xFF00 + _registers.get_c(), _registers.get_a());

        return 8;
    case 0xE5:
        //cout << "PUSH HL" << endl;

        _push_stack(_registers.get_hl());

        return 16;
    case 0xE6:
        //cout << "AND A,u8" << endl;

        _registers.set_a(_and(_registers.get_a(), _read8()));

        return 8;
    case 0xE7:
        //cout << "RST 20h" << endl;

        _rst(0x20);

        return 16;
    case 0xE9:
        //cout << "JP HL" << endl;

        _registers.set_pc(_registers.get_hl());

        break;
    case 0xEA:
        //cout << "LD (u16),A" << endl;

        next_u16 = _read16();
        _memory.write(next_u16, _registers.get_a());

        return 16;
    case 0xEE:
    {
        //cout << "XOR A,u8" << endl;

        u8 res = _registers.get_a() ^ _read8();
        _registers.set_a(res);

        _registers.set_flag_z(res == 0);
        _registers.set_flag_z(0);
        _registers.set_flag_h(0);
        _registers.set_flag_c(0);

        return 8;
    }
    case 0xEF:
        //cout << "RST 28h" << endl;

        _rst(0x28);

        return 16;
    case 0xF0:
        //cout << "LD A,(FF00+u8)" << endl;

        next_u8 = _read8();
        _registers.set_a(_memory.read(get_u16(next_u8, 0xFF)));

        return 12;
    case 0xF1:
        //cout << "POP AF" << endl;

        _registers.set_af(_pop_stack());

        return 12;
    case 0xF3:
        //cout << "DI" << endl;
        
        _ime = false;

        break;
    case 0xF5:
        //cout << "PUSH AF" << endl;
            
        _push_stack(_registers.get_af());

        return 16;
    case 0xF6:
        //cout << "OR A,u8" << endl;

        _registers.set_a(_or(_registers.get_a(), _read8()));

        return 8;
    case 0xF7:
        //cout << "RST 30h" << endl;

        _rst(0x30);

        return 16;
    case 0xF8:
    {
        //cout << "LD HL,SP+i8" << endl;

        u16 sp = _registers.get_sp();
        i8 next_i8 = (i8)_read8();
        unsigned int res = sp + next_i8;

        _registers.set_hl((u16)res);

        _registers.set_flag_z(0);
        _registers.set_flag_n(0);
        _registers.set_flag_h((sp & 0xF) + (next_i8 & 0xF) > 0xF);
        _registers.set_flag_c(res > 0xFF);

        return 12;
    }
    case 0xF9:
        //cout << "LD SP,HL" << endl;

        _registers.set_sp(_registers.get_hl());

        return 8;
    case 0xFA:
        //cout << "LD A,(u16)" << endl;

        next_u16 = _read16();
        _registers.set_a(_memory.read(next_u16));

        return 16;
    case 0xFB:
        //cout << "EI" << endl;

        _ime = true;

        break;
    case 0xFD:
        //cout << "Undefined" << endl;

        break;
    case 0xFE:
    {
        //cout << "CP A,u8" << endl;

        _cp(_registers.get_a(), _read8());

        return 8;
    }
    case 0xFF:
        //cout << "RST 38h" << endl;

        _rst(0x38);

        return 16;
    case 0xCB:  // Extended opcode
    {
        u8 extended_opcode = _memory.read(_registers.get_pc());
        _registers.inc_pc();

        // printf("%X ", extended_opcode);

        switch (extended_opcode) {
        case 0x18:
            //cout << "RR B" << endl;

            _registers.set_b(_rr(_registers.get_b()));

            break;
        case 0x19:
            //cout << "RR C" << endl;

            _registers.set_c(_rr(_registers.get_c()));

            break;
        case 0x1A:
            //cout << "RR D" << endl;

            _registers.set_d(_rr(_registers.get_d()));

            break;
        case 0x1B:
            //cout << "RR E" << endl;

            _registers.set_e(_rr(_registers.get_e()));

            break;
        case 0x1C:
            //cout << "RR H" << endl;

            _registers.set_h(_rr(_registers.get_h()));

            break;
        case 0x1D:
            //cout << "RR L" << endl;

            _registers.set_l(_rr(_registers.get_l()));

            break;
        case 0x1E:
        {
            //cout << "RR (HL)" << endl;

            u16 hl_addr = _registers.get_hl();
            _memory.write(hl_addr, _rr(_memory.read(hl_addr)));

            return 16;
        }
        case 0x1F:
            //cout << "RR A" << endl;

            _registers.set_a(_rr(_registers.get_a()));

            break;
        case 0x20:
            //cout << "SLA B" << endl;

            _registers.set_b(_sla(_registers.get_b()));

            break;
        case 0x21:
            //cout << "SLA C" << endl;

            _registers.set_c(_sla(_registers.get_c()));

            break;
        case 0x22:
            //cout << "SLA D" << endl;

            _registers.set_d(_sla(_registers.get_d()));

            break;
        case 0x23:
            //cout << "SLA E" << endl;

            _registers.set_e(_sla(_registers.get_e()));

            break;
        case 0x24:
            //cout << "SLA H" << endl;

            _registers.set_h(_sla(_registers.get_h()));

            break;
        case 0x25:
            //cout << "SLA L" << endl;

            _registers.set_l (_sla(_registers.get_l()));

            break;
        case 0x26:
        {
            //cout << "SLA (HL)" << endl;

            u16 hl_addr = _registers.get_hl();

            _memory.write(hl_addr, _sla(_memory.read(hl_addr)));

            return 16;
        }
        case 0x27:
            //cout << "SLA A" << endl;

            _registers.set_a(_sla(_registers.get_a()));

            break;
        case 0x30:
            //cout << "SWAP B" << endl;

            _registers.set_b(_swap(_registers.get_b()));

            break;
        case 0x31:
            //cout << "SWAP C" << endl;

            _registers.set_c(_swap(_registers.get_c()));

            break;
        case 0x32:
            //cout << "SWAP D" << endl;

            _registers.set_d(_swap(_registers.get_d()));

            break;
        case 0x33:
            //cout << "SWAP E" << endl;

            _registers.set_e(_swap(_registers.get_e()));

            break;
        case 0x34:
            //cout << "SWAP H" << endl;

            _registers.set_h(_swap(_registers.get_h()));

            break;
        case 0x35:
            //cout << "SWAP L" << endl;

            _registers.set_l(_swap(_registers.get_l ()));

            break;
        case 0x36:
        {
            //cout << "SWAP (HL)" << endl;

            u16 hl_addr = _registers.get_hl();
            
            _memory.write(hl_addr, _swap(_memory.read(hl_addr)));

            return 16;
        }
        case 0x37:
            //cout << "SWAP A" << endl;

            _registers.set_a(_swap(_registers.get_a()));

            break;
        case 0x38:
            //cout << "SRL B" << endl;

            _registers.set_b(_srl(_registers.get_b()));

            break;
        case 0x39:
            //cout << "SRL C" << endl;

            _registers.set_c(_srl(_registers.get_c()));

            break;
        case 0x3A:
            //cout << "SRL D" << endl;

            _registers.set_d(_srl(_registers.get_d()));

            break;
        case 0x3B:
            //cout << "SRL E" << endl;

            _registers.set_e(_srl(_registers.get_e()));

            break;
        case 0x3C:
            //cout << "SRL H" << endl;

            _registers.set_h(_srl(_registers.get_h()));

            break;
        case 0x3D:
            //cout << "SRL L" << endl;

            _registers.set_l(_srl(_registers.get_l()));

            break;
        case 0x3E:
        {
            //cout << "SRL (HL)" << endl;

            u16 hl_addr = _registers.get_hl();

            _memory.write(hl_addr, _srl(_memory.read(hl_addr)));

            return 16;
        }
        case 0x3F:
            //cout << "SRL A" << endl;

            _registers.set_a(_srl(_registers.get_a()));

            break;
        case 0x40:
            //cout << "BIT 0,B" << endl;

            _registers.set_flag_z(get_bit(_registers.get_b(), 0) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x41:
            //cout << "BIT 0,C" << endl;

            _registers.set_flag_z(get_bit(_registers.get_c(), 0) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x42:
            //cout << "BIT 0,D" << endl;

            _registers.set_flag_z(get_bit(_registers.get_d(), 0) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x43:
            //cout << "BIT 0,E" << endl;

            _registers.set_flag_z(get_bit(_registers.get_e(), 0) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x44:
            //cout << "BIT 0,H" << endl;

            _registers.set_flag_z(get_bit(_registers.get_h(), 0) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x45:
            //cout << "BIT 0,L" << endl;

            _registers.set_flag_z(get_bit(_registers.get_l(), 0) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x46:
            //cout << "BIT 0,(HL)" << endl;

            _registers.set_flag_z(get_bit(_memory.read(_registers.get_hl()), 0) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            return 12;
        case 0x47:
            //cout << "BIT 0,A" << endl;

            _registers.set_flag_z(get_bit(_registers.get_a(), 0) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x48:
            //cout << "BIT 1,B" << endl;

            _registers.set_flag_z(get_bit(_registers.get_b(), 1) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x49:
            //cout << "BIT 1,C" << endl;

            _registers.set_flag_z(get_bit(_registers.get_c(), 1) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x4A:
            //cout << "BIT 1,D" << endl;

            _registers.set_flag_z(get_bit(_registers.get_d(), 1) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x4B:
            //cout << "BIT 1,E" << endl;

            _registers.set_flag_z(get_bit(_registers.get_e(), 1) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x4C:
            //cout << "BIT 1,H" << endl;

            _registers.set_flag_z(get_bit(_registers.get_h(), 1) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x4D:
            //cout << "BIT 1,L" << endl;

            _registers.set_flag_z(get_bit(_registers.get_l(), 1) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x4E:
            //cout << "BIT 1,(HL)" << endl;

            _registers.set_flag_z(get_bit(_memory.read(_registers.get_hl()), 1) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            return 12;
        case 0x4F:
            //cout << "BIT 1,A" << endl;

            _registers.set_flag_z(get_bit(_registers.get_a(), 1) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x50:
            //cout << "BIT 2,B" << endl;

            _registers.set_flag_z(get_bit(_registers.get_b(), 2) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x51:
            //cout << "BIT 2,C" << endl;

            _registers.set_flag_z(get_bit(_registers.get_c(), 2) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x52:
            //cout << "BIT 2,D" << endl;

            _registers.set_flag_z(get_bit(_registers.get_d(), 2) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x53:
            //cout << "BIT 2,E" << endl;

            _registers.set_flag_z(get_bit(_registers.get_e(), 2) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x54:
            //cout << "BIT 2,H" << endl;

            _registers.set_flag_z(get_bit(_registers.get_h(), 2) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x55:
            //cout << "BIT 2,L" << endl;

            _registers.set_flag_z(get_bit(_registers.get_l(), 2) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x56:
            //cout << "BIT 2,(HL)" << endl;

            _registers.set_flag_z(get_bit(_memory.read(_registers.get_hl()), 2) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            return 12;
        case 0x57:
            //cout << "BIT 2,A" << endl;

            _registers.set_flag_z(get_bit(_registers.get_a(), 2) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x58:
            //cout << "BIT 3,B" << endl;

            _registers.set_flag_z(get_bit(_registers.get_b(), 3) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x59:
            //cout << "BIT 3,C" << endl;

            _registers.set_flag_z(get_bit(_registers.get_c(), 3) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x5A:
            //cout << "BIT 3,D" << endl;

            _registers.set_flag_z(get_bit(_registers.get_d(), 3) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x5B:
            //cout << "BIT 3,E" << endl;

            _registers.set_flag_z(get_bit(_registers.get_e(), 3) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x5C:
            //cout << "BIT 3,H" << endl;

            _registers.set_flag_z(get_bit(_registers.get_h(), 3) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x5D:
            //cout << "BIT 3,L" << endl;

            _registers.set_flag_z(get_bit(_registers.get_l(), 3) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x5E:
            //cout << "BIT 3,(HL)" << endl;

            _registers.set_flag_z(get_bit(_memory.read(_registers.get_hl()), 3) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            return 12;
        case 0x5F:
            //cout << "BIT 3,A" << endl;

            _registers.set_flag_z(get_bit(_registers.get_a(), 3) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x60:
            //cout << "BIT 4,B" << endl;

            _registers.set_flag_z(get_bit(_registers.get_b(), 4) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x61:
            //cout << "BIT 4,C" << endl;

            _registers.set_flag_z(get_bit(_registers.get_c(), 4) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x62:
            //cout << "BIT 4,D" << endl;

            _registers.set_flag_z(get_bit(_registers.get_d(), 4) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x63:
            //cout << "BIT 4,E" << endl;

            _registers.set_flag_z(get_bit(_registers.get_e(), 4) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x64:
            //cout << "BIT 4,H" << endl;

            _registers.set_flag_z(get_bit(_registers.get_h(), 4) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x65:
            //cout << "BIT 4,L" << endl;

            _registers.set_flag_z(get_bit(_registers.get_l(), 4) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x66:
            //cout << "BIT 4,(HL)" << endl;

            _registers.set_flag_z(get_bit(_memory.read(_registers.get_hl()), 4) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            return 12;
        case 0x67:
            //cout << "BIT 4,A" << endl;

            _registers.set_flag_z(get_bit(_registers.get_a(), 4) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x68:
            //cout << "BIT 5,B" << endl;

            _registers.set_flag_z(get_bit(_registers.get_b(), 5) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x69:
            //cout << "BIT 5,C" << endl;

            _registers.set_flag_z(get_bit(_registers.get_c(), 5) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x6A:
            //cout << "BIT 5,D" << endl;

            _registers.set_flag_z(get_bit(_registers.get_d(), 5) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x6B:
            //cout << "BIT 5,E" << endl;

            _registers.set_flag_z(get_bit(_registers.get_e(), 5) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x6C:
            //cout << "BIT 5,H" << endl;

            _registers.set_flag_z(get_bit(_registers.get_h(), 5) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x6D:
            //cout << "BIT 5,L" << endl;

            _registers.set_flag_z(get_bit(_registers.get_l(), 5) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x6E:
            //cout << "BIT 5,(HL)" << endl;

            _registers.set_flag_z(get_bit(_memory.read(_registers.get_hl()), 5) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            return 12;
        case 0x6F:
            //cout << "BIT 5,A" << endl;

            _registers.set_flag_z(get_bit(_registers.get_a(), 5) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x70:
            //cout << "BIT 6,B" << endl;

            _registers.set_flag_z(get_bit(_registers.get_b(), 6) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x71:
            //cout << "BIT 6,C" << endl;

            _registers.set_flag_z(get_bit(_registers.get_c(), 6) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x72:
            //cout << "BIT 6,D" << endl;

            _registers.set_flag_z(get_bit(_registers.get_d(), 6) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x73:
            //cout << "BIT 6,E" << endl;

            _registers.set_flag_z(get_bit(_registers.get_e(), 6) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x74:
            //cout << "BIT 6,H" << endl;

            _registers.set_flag_z(get_bit(_registers.get_h(), 6) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x75:
            //cout << "BIT 6,L" << endl;

            _registers.set_flag_z(get_bit(_registers.get_l(), 6) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x76:
            //cout << "BIT 6,(HL)" << endl;

            _registers.set_flag_z(get_bit(_memory.read(_registers.get_hl()), 6) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            return 12;
        case 0x77:
            //cout << "BIT 6,A" << endl;

            _registers.set_flag_z(get_bit(_registers.get_a(), 6) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x78:
            //cout << "BIT 7,B" << endl;

            _registers.set_flag_z(get_bit(_registers.get_b(), 7) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x79:
            //cout << "BIT 7,C" << endl;

            _registers.set_flag_z(get_bit(_registers.get_c(), 7) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x7A:
            //cout << "BIT 7,D" << endl;

            _registers.set_flag_z(get_bit(_registers.get_d(), 7) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x7B:
            //cout << "BIT 7,E" << endl;

            _registers.set_flag_z(get_bit(_registers.get_e(), 7) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x7C:
            //cout << "BIT 7,H" << endl;

            _registers.set_flag_z(get_bit(_registers.get_h(), 7) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x7D:
            //cout << "BIT 7,L" << endl;

            _registers.set_flag_z(get_bit(_registers.get_l(), 7) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x7E:
            //cout << "BIT 7,(HL)" << endl;

            _registers.set_flag_z(get_bit(_memory.read(_registers.get_hl()), 7) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            return 12;
        case 0x7F:
            //cout << "BIT 7,A" << endl;

            _registers.set_flag_z(get_bit(_registers.get_a(), 7) == 0);
            _registers.set_flag_n(0);
            _registers.set_flag_h(1);

            break;
        case 0x80:
            //cout << "RES 0,B" << endl;

            _registers.set_b(set_bit(_registers.get_b(), 0, 0));

            break;
        case 0x81:
            //cout << "RES 0,C" << endl;

            _registers.set_c(set_bit(_registers.get_c(), 0, 0));

            break;
        case 0x82:
            //cout << "RES 0,D" << endl;

            _registers.set_d(set_bit(_registers.get_d(), 0, 0));

            break;
        case 0x83:
            //cout << "RES 0,E" << endl;

            _registers.set_e(set_bit(_registers.get_e(), 0, 0));

            break;
        case 0x84:
            //cout << "RES 0,H" << endl;

            _registers.set_h(set_bit(_registers.get_h(), 0, 0));

            break;
        case 0x85:
            //cout << "RES 0,L" << endl;

            _registers.set_l(set_bit(_registers.get_l(), 0, 0));

            break;
        case 0x86:
        {
            //cout << "RES 0,(HL)" << endl;

            u16 hl_addr = _registers.get_hl();
            _memory.write(hl_addr, set_bit(_memory.read(hl_addr), 0, 0));

            return 16;
        }
        case 0x87:
            //cout << "RES 0,A" << endl;

            _registers.set_a(set_bit(_registers.get_a(), 0, 0));

            break;
        case 0x88:
            //cout << "RES 1,B" << endl;

            _registers.set_b(set_bit(_registers.get_b(), 1, 0));

            break;
        case 0x89:
            //cout << "RES 1,C" << endl;

            _registers.set_c(set_bit(_registers.get_c(), 1, 0));

            break;
        case 0x8A:
            //cout << "RES 1,D" << endl;

            _registers.set_d(set_bit(_registers.get_d(), 1, 0));

            break;
        case 0x8B:
            //cout << "RES 1,E" << endl;

            _registers.set_e(set_bit(_registers.get_e(), 1, 0));

            break;
        case 0x8C:
            //cout << "RES 1,H" << endl;

            _registers.set_h(set_bit(_registers.get_h(), 1, 0));

            break;
        case 0x8D:
            //cout << "RES 1,L" << endl;

            _registers.set_l(set_bit(_registers.get_l(), 1, 0));

            break;
        case 0x8E:
        {
            //cout << "RES 1,(HL)" << endl;

            u16 hl_addr = _registers.get_hl();
            _memory.write(hl_addr, set_bit(_memory.read(hl_addr), 1, 0));

            return 16;
        }
        case 0x8F:
            //cout << "RES 1,A" << endl;

            _registers.set_a(set_bit(_registers.get_a(), 1, 0));

            break;
        case 0x90:
            //cout << "RES 2,B" << endl;

            _registers.set_b(set_bit(_registers.get_b(), 2, 0));

            break;
        case 0x91:
            //cout << "RES 2,C" << endl;

            _registers.set_c(set_bit(_registers.get_c(), 2, 0));

            break;
        case 0x92:
            //cout << "RES 2,D" << endl;

            _registers.set_d(set_bit(_registers.get_d(), 2, 0));

            break;
        case 0x93:
            //cout << "RES 2,E" << endl;

            _registers.set_e(set_bit(_registers.get_e(), 2, 0));

            break;
        case 0x94:
            //cout << "RES 2,H" << endl;

            _registers.set_h(set_bit(_registers.get_h(), 2, 0));

            break;
        case 0x95:
            //cout << "RES 2,L" << endl;

            _registers.set_l(set_bit(_registers.get_l(), 2, 0));

            break;
        case 0x96:
        {
            //cout << "RES 2,(HL)" << endl;

            u16 hl_addr = _registers.get_hl();
            _memory.write(hl_addr, set_bit(_memory.read(hl_addr), 2, 0));

            return 16;
        }
        case 0x97:
            //cout << "RES 2,A" << endl;

            _registers.set_a(set_bit(_registers.get_a(), 2, 0));

            break;
        case 0x98:
            //cout << "RES 3,B" << endl;

            _registers.set_b(set_bit(_registers.get_b(), 3, 0));

            break;
        case 0x99:
            //cout << "RES 3,C" << endl;

            _registers.set_c(set_bit(_registers.get_c(), 3, 0));

            break;
        case 0x9A:
            //cout << "RES 3,D" << endl;

            _registers.set_d(set_bit(_registers.get_d(), 3, 0));

            break;
        case 0x9B:
            //cout << "RES 3,E" << endl;

            _registers.set_e(set_bit(_registers.get_e(), 3, 0));

            break;
        case 0x9C:
            //cout << "RES 3,H" << endl;

            _registers.set_h(set_bit(_registers.get_h(), 3, 0));

            break;
        case 0x9D:
            //cout << "RES 3,L" << endl;

            _registers.set_l(set_bit(_registers.get_l(), 3, 0));

            break;
        case 0x9E:
        {
            //cout << "RES 3,(HL)" << endl;

            u16 hl_addr = _registers.get_hl();
            _memory.write(hl_addr, set_bit(_memory.read(hl_addr), 3, 0));

            return 16;
        }
        case 0x9F:
            //cout << "RES 3,A" << endl;

            _registers.set_a(set_bit(_registers.get_a(), 3, 0));

            break;
        case 0xA0:
            //cout << "RES 4,B" << endl;

            _registers.set_b(set_bit(_registers.get_b(), 4, 0));

            break;
        case 0xA1:
            //cout << "RES 4,C" << endl;

            _registers.set_c(set_bit(_registers.get_c(), 4, 0));

            break;
        case 0xA2:
            //cout << "RES 4,D" << endl;

            _registers.set_d(set_bit(_registers.get_d(), 4, 0));

            break;
        case 0xA3:
            //cout << "RES 4,E" << endl;

            _registers.set_e(set_bit(_registers.get_e(), 4, 0));

            break;
        case 0xA4:
            //cout << "RES 4,H" << endl;

            _registers.set_h(set_bit(_registers.get_h(), 4, 0));

            break;
        case 0xA5:
            //cout << "RES 4,L" << endl;

            _registers.set_l(set_bit(_registers.get_l(), 4, 0));

            break;
        case 0xA6:
        {
            //cout << "RES 4,(HL)" << endl;

            u16 hl_addr = _registers.get_hl();
            _memory.write(hl_addr, set_bit(_memory.read(hl_addr), 4, 0));

            return 16;
        }
        case 0xA7:
            //cout << "RES 4,A" << endl;

            _registers.set_a(set_bit(_registers.get_a(), 4, 0));

            break;
        case 0xA8:
            //cout << "RES 5,B" << endl;

            _registers.set_b(set_bit(_registers.get_b(), 5, 0));

            break;
        case 0xA9:
            //cout << "RES 5,C" << endl;

            _registers.set_c(set_bit(_registers.get_c(), 5, 0));

            break;
        case 0xAA:
            //cout << "RES 5,D" << endl;

            _registers.set_d(set_bit(_registers.get_d(), 5, 0));

            break;
        case 0xAB:
            //cout << "RES 5,E" << endl;

            _registers.set_e(set_bit(_registers.get_e(), 5, 0));

            break;
        case 0xAC:
            //cout << "RES 5,H" << endl;

            _registers.set_h(set_bit(_registers.get_h(), 5, 0));

            break;
        case 0xAD:
            //cout << "RES 5,L" << endl;

            _registers.set_l(set_bit(_registers.get_l(), 5, 0));

            break;
        case 0xAE:
        {
            //cout << "RES 5,(HL)" << endl;

            u16 hl_addr = _registers.get_hl();
            _memory.write(hl_addr, set_bit(_memory.read(hl_addr), 5, 0));

            return 16;
        }
        case 0xAF:
            //cout << "RES 5,A" << endl;

            _registers.set_a(set_bit(_registers.get_a(), 5, 0));

            break;
        case 0xB0:
            //cout << "RES 6,B" << endl;

            _registers.set_b(set_bit(_registers.get_b(), 6, 0));

            break;
        case 0xB1:
            //cout << "RES 6,C" << endl;

            _registers.set_c(set_bit(_registers.get_c(), 6, 0));

            break;
        case 0xB2:
            //cout << "RES 6,D" << endl;

            _registers.set_d(set_bit(_registers.get_d(), 6, 0));

            break;
        case 0xB3:
            //cout << "RES 6,E" << endl;

            _registers.set_e(set_bit(_registers.get_e(), 6, 0));

            break;
        case 0xB4:
            //cout << "RES 6,H" << endl;

            _registers.set_h(set_bit(_registers.get_h(), 6, 0));

            break;
        case 0xB5:
            //cout << "RES 6,L" << endl;

            _registers.set_l(set_bit(_registers.get_l(), 6, 0));

            break;
        case 0xB6:
        {
            //cout << "RES 6,(HL)" << endl;

            u16 hl_addr = _registers.get_hl();
            _memory.write(hl_addr, set_bit(_memory.read(hl_addr), 6, 0));

            return 16;
        }
        case 0xB7:
            //cout << "RES 6,A" << endl;

            _registers.set_a(set_bit(_registers.get_a(), 6, 0));

            break;
        case 0xB8:
            //cout << "RES 7,B" << endl;

            _registers.set_b(set_bit(_registers.get_b(), 7, 0));

            break;
        case 0xB9:
            //cout << "RES 7,C" << endl;

            _registers.set_c(set_bit(_registers.get_c(), 7, 0));

            break;
        case 0xBA:
            //cout << "RES 7,D" << endl;

            _registers.set_d(set_bit(_registers.get_d(), 7, 0));

            break;
        case 0xBB:
            //cout << "RES 7,E" << endl;

            _registers.set_e(set_bit(_registers.get_e(), 7, 0));

            break;
        case 0xBC:
            //cout << "RES 7,H" << endl;

            _registers.set_h(set_bit(_registers.get_h(), 7, 0));

            break;
        case 0xBD:
            //cout << "RES 7,L" << endl;

            _registers.set_l(set_bit(_registers.get_l(), 7, 0));

            break;
        case 0xBE:
        {
            //cout << "RES 7,(HL)" << endl;

            u16 hl_addr = _registers.get_hl();
            _memory.write(hl_addr, set_bit(_memory.read(hl_addr), 7, 0));

            return 16;
        }
        case 0xBF:
            //cout << "RES 7,A" << endl;

            _registers.set_a(set_bit(_registers.get_a(), 7, 0));

            break;
        case 0xC0:
            //cout << "SET 0,B" << endl;

            _registers.set_b(set_bit(_registers.get_b(), 0, 1));

            break;
        case 0xC1:
            //cout << "SET 0,C" << endl;

            _registers.set_c(set_bit(_registers.get_c(), 0, 1));

            break;
        case 0xC2:
            //cout << "SET 0,D" << endl;

            _registers.set_d(set_bit(_registers.get_d(), 0, 1));

            break;
        case 0xC3:
            //cout << "SET 0,E" << endl;

            _registers.set_e(set_bit(_registers.get_e(), 0, 1));

            break;
        case 0xC4:
            //cout << "SET 0,H" << endl;

            _registers.set_h(set_bit(_registers.get_h(), 0, 1));

            break;
        case 0xC5:
            //cout << "SET 0,L" << endl;

            _registers.set_l(set_bit(_registers.get_l(), 0, 1));

            break;
        case 0xC6:
        {
            //cout << "SET 0,(HL)" << endl;

            u16 hl_addr = _registers.get_hl();
            _memory.write(hl_addr, set_bit(_memory.read(hl_addr), 0, 1));

            return 16;
        }
        case 0xC7:
            //cout << "SET 0,A" << endl;

            _registers.set_a(set_bit(_registers.get_a(), 0, 1));

            break;
        case 0xC8:
            //cout << "SET 1,B" << endl;

            _registers.set_b(set_bit(_registers.get_b(), 1, 1));

            break;
        case 0xC9:
            //cout << "SET 1,C" << endl;

            _registers.set_c(set_bit(_registers.get_c(), 1, 1));

            break;
        case 0xCA:
            //cout << "SET 1,D" << endl;

            _registers.set_d(set_bit(_registers.get_d(), 1, 1));

            break;
        case 0xCB:
            //cout << "SET 1,E" << endl;

            _registers.set_e(set_bit(_registers.get_e(), 1, 1));

            break;
        case 0xCC:
            //cout << "SET 1,H" << endl;

            _registers.set_h(set_bit(_registers.get_h(), 1, 1));

            break;
        case 0xCD:
            //cout << "SET 1,L" << endl;

            _registers.set_l(set_bit(_registers.get_l(), 1, 1));

            break;
        case 0xCE:
        {
            //cout << "SET 1,(HL)" << endl;

            u16 hl_addr = _registers.get_hl();
            _memory.write(hl_addr, set_bit(_memory.read(hl_addr), 1, 1));

            return 16;
        }
        case 0xCF:
            //cout << "SET 1,A" << endl;

            _registers.set_a(set_bit(_registers.get_a(), 1, 1));

            break;
        case 0xD0:
            //cout << "SET 2,B" << endl;

            _registers.set_b(set_bit(_registers.get_b(), 2, 1));

            break;
        case 0xD1:
            //cout << "SET 2,C" << endl;

            _registers.set_c(set_bit(_registers.get_c(), 2, 1));

            break;
        case 0xD2:
            //cout << "SET 2,D" << endl;

            _registers.set_d(set_bit(_registers.get_d(), 2, 1));

            break;
        case 0xD3:
            //cout << "SET 2,E" << endl;

            _registers.set_e(set_bit(_registers.get_e(), 2, 1));

            break;
        case 0xD4:
            //cout << "SET 2,H" << endl;

            _registers.set_h(set_bit(_registers.get_h(), 2, 1));

            break;
        case 0xD5:
            //cout << "SET 2,L" << endl;

            _registers.set_l(set_bit(_registers.get_l(), 2, 1));

            break;
        case 0xD6:
        {
            //cout << "SET 2,(HL)" << endl;

            u16 hl_addr = _registers.get_hl();
            _memory.write(hl_addr, set_bit(_memory.read(hl_addr), 2, 1));

            return 16;
        }
        case 0xD7:
            //cout << "SET 2,A" << endl;

            _registers.set_a(set_bit(_registers.get_a(), 2, 1));

            break;
        case 0xD8:
            //cout << "SET 3,B" << endl;

            _registers.set_b(set_bit(_registers.get_b(), 3, 1));

            break;
        case 0xD9:
            //cout << "SET 3,C" << endl;

            _registers.set_c(set_bit(_registers.get_c(), 3, 1));

            break;
        case 0xDA:
            //cout << "SET 3,D" << endl;

            _registers.set_d(set_bit(_registers.get_d(), 3, 1));

            break;
        case 0xDB:
            //cout << "SET 3,E" << endl;

            _registers.set_e(set_bit(_registers.get_e(), 3, 1));

            break;
        case 0xDC:
            //cout << "SET 3,H" << endl;

            _registers.set_h(set_bit(_registers.get_h(), 3, 1));

            break;
        case 0xDD:
            //cout << "SET 3,L" << endl;

            _registers.set_l(set_bit(_registers.get_l(), 3, 1));

            break;
        case 0xDE:
        {
            //cout << "SET 3,(HL)" << endl;

            u16 hl_addr = _registers.get_hl();
            _memory.write(hl_addr, set_bit(_memory.read(hl_addr), 3, 1));

            return 16;
        }
        case 0xDF:
            //cout << "SET 3,A" << endl;

            _registers.set_a(set_bit(_registers.get_a(), 3, 1));

            break;
        case 0xE0:
            //cout << "SET 4,B" << endl;

            _registers.set_b(set_bit(_registers.get_b(), 4, 1));

            break;
        case 0xE1:
            //cout << "SET 4,C" << endl;

            _registers.set_c(set_bit(_registers.get_c(), 4, 1));

            break;
        case 0xE2:
            //cout << "SET 4,D" << endl;

            _registers.set_d(set_bit(_registers.get_d(), 4, 1));

            break;
        case 0xE3:
            //cout << "SET 4,E" << endl;

            _registers.set_e(set_bit(_registers.get_e(), 4, 1));

            break;
        case 0xE4:
            //cout << "SET 4,H" << endl;

            _registers.set_h(set_bit(_registers.get_h(), 4, 1));

            break;
        case 0xE5:
            //cout << "SET 4,L" << endl;

            _registers.set_l(set_bit(_registers.get_l(), 4, 1));

            break;
        case 0xE6:
        {
            //cout << "SET 4,(HL)" << endl;

            u16 hl_addr = _registers.get_hl();
            _memory.write(hl_addr, set_bit(_memory.read(hl_addr), 4, 1));

            return 16;
        }
        case 0xE7:
            //cout << "SET 4,A" << endl;

            _registers.set_a(set_bit(_registers.get_a(), 4, 1));

            break;
        case 0xE8:
            //cout << "SET 5,B" << endl;

            _registers.set_b(set_bit(_registers.get_b(), 5, 1));

            break;
        case 0xE9:
            //cout << "SET 5,C" << endl;

            _registers.set_c(set_bit(_registers.get_c(), 5, 1));

            break;
        case 0xEA:
            //cout << "SET 5,D" << endl;

            _registers.set_d(set_bit(_registers.get_d(), 5, 1));

            break;
        case 0xEB:
            //cout << "SET 5,E" << endl;

            _registers.set_e(set_bit(_registers.get_e(), 5, 1));

            break;
        case 0xEC:
            //cout << "SET 5,H" << endl;

            _registers.set_h(set_bit(_registers.get_h(), 5, 1));

            break;
        case 0xED:
            //cout << "SET 5,L" << endl;

            _registers.set_l(set_bit(_registers.get_l(), 5, 1));

            break;
        case 0xEE:
        {
            //cout << "SET 5,(HL)" << endl;

            u16 hl_addr = _registers.get_hl();
            _memory.write(hl_addr, set_bit(_memory.read(hl_addr), 5, 1));

            return 16;
        }
        case 0xEF:
            //cout << "SET 5,A" << endl;

            _registers.set_a(set_bit(_registers.get_a(), 5, 1));

            break;
        case 0xF0:
            //cout << "SET 6,B" << endl;

            _registers.set_b(set_bit(_registers.get_b(), 6, 1));

            break;
        case 0xF1:
            //cout << "SET 6,C" << endl;

            _registers.set_c(set_bit(_registers.get_c(), 6, 1));

            break;
        case 0xF2:
            //cout << "SET 6,D" << endl;

            _registers.set_d(set_bit(_registers.get_d(), 6, 1));

            break;
        case 0xF3:
            //cout << "SET 6,E" << endl;

            _registers.set_e(set_bit(_registers.get_e(), 6, 1));

            break;
        case 0xF4:
            //cout << "SET 6,H" << endl;

            _registers.set_h(set_bit(_registers.get_h(), 6, 1));

            break;
        case 0xF5:
            //cout << "SET 6,L" << endl;

            _registers.set_l(set_bit(_registers.get_l(), 6, 1));

            break;
        case 0xF6:
        {
            //cout << "SET 6,(HL)" << endl;

            u16 hl_addr = _registers.get_hl();
            _memory.write(hl_addr, set_bit(_memory.read(hl_addr), 6, 1));

            return 16;
        }
        case 0xF7:
            //cout << "SET 6,A" << endl;

            _registers.set_a(set_bit(_registers.get_a(), 6, 1));

            break;
        case 0xF8:
            //cout << "SET 7,B" << endl;

            _registers.set_b(set_bit(_registers.get_b(), 7, 1));

            break;
        case 0xF9:
            //cout << "SET 7,C" << endl;

            _registers.set_c(set_bit(_registers.get_c(), 7, 1));

            break;
        case 0xFA:
            //cout << "SET 7,D" << endl;

            _registers.set_d(set_bit(_registers.get_d(), 7, 1));

            break;
        case 0xFB:
            //cout << "SET 7,E" << endl;

            _registers.set_e(set_bit(_registers.get_e(), 7, 1));

            break;
        case 0xFC:
            //cout << "SET 7,H" << endl;

            _registers.set_h(set_bit(_registers.get_h(), 7, 1));

            break;
        case 0xFD:
            //cout << "SET 7,L" << endl;

            _registers.set_l(set_bit(_registers.get_l(), 7, 1));

            break;
        case 0xFE:
        {
            //cout << "SET 7,(HL)" << endl;

            u16 hl_addr = _registers.get_hl();
            _memory.write(hl_addr, set_bit(_memory.read(hl_addr), 7, 1));

            return 16;
        }
        case 0xFF:
            //cout << "SET 7,A" << endl;

            _registers.set_a(set_bit(_registers.get_a(), 7, 1));

            break;
        default:
             cout << "Unrecognized (extended) opcode" << endl;
        }

        return 8;
    }
    default:
         cout << "Unrecognized opcode" << endl;
    }

    //// printf("pc: %X ", _registers.get_pc());

    // TODO: Return actual number of cycles
    return 4;
}


void Cpu::handle_interrupt() {
    // If CPU has been halted, we need to check if an interrupt is requested even if
    // IME is disabled to unhalt it.
    if (_ime || !is_on) {
        u8 interrupt_flag = _memory.read(0xFF0F);
        u8 interrupt_enable = _memory.read(0xFFFF);

        // Service the interrupt in the order of priority
        for (int i = 0; i <= 4; i++) {
            if (get_bit(interrupt_flag, i) && get_bit(interrupt_enable, i)) {
                is_on = true;

                if (_ime) {
                    Cpu::service_interrupt(i);
                }
            }
        }
    }
}


// interrupt_bit can take on 0 (V-Blank), 1 (LCDC Status), ..., 4.
void Cpu::service_interrupt(u3 interrupt_bit) {
    _ime = false;

    // Reset the IF register
    _memory.write(0xFF0F, set_bit(_memory.read(0xFF0F), interrupt_bit, 0));

    _push_stack(_registers.get_pc());

    // Jump to starting address of interrupt
    switch (interrupt_bit) {
    case 0:
        _registers.set_pc(0x40);
        break;
    case 1:
        _registers.set_pc(0x48);
        break;
    case 2:
        _registers.set_pc(0x50);
        break;
    case 4:
        _registers.set_pc(0x60);
        break;
    }
}


/* For reading ROM file directly - must replace later */

// Reads the next 8 bits from memory at pc address and increments pc.
u8 Cpu::_read8() {
    u8 u8 = _memory.read(_registers.get_pc());
    _registers.inc_pc();

    return u8;
}

// Reads the next 16 bits from memroy at pc address and increments pc
// twice.
u16 Cpu::_read16() {
    // Read the next two bytes
    u8 lsb = _memory.read(_registers.get_pc());
    _registers.inc_pc();
    u8 msb = _memory.read(_registers.get_pc());
    _registers.inc_pc();

    // Return the 16-bit address
    return get_u16(lsb, msb);
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


// ADC A,n - Add n + Carry flag to A.
// Sets flags appropriately.
void Cpu::_adc_a(const u8 n) {
    u8 a = _registers.get_a();
    bool flag_c = _registers.get_flag_c();
    int res = a + n + flag_c;

    _registers.set_flag_z((u8)res == 0);
    _registers.set_flag_n(0);
    _registers.set_flag_h((a & 0xF) + (n & 0xF) + flag_c > 0xF);
    _registers.set_flag_c(res > 0xFF);

    _registers.set_a((u8)res);
}

// Adds val1 and val2 and returns the value, and sets flags appropriately.
// Used for ADD A,n
u8 Cpu::_add(const u8 val1, const u8 val2) {
    u16 res = val1 + val2;

    _registers.set_flag_z((u8)res == 0);
    _registers.set_flag_n(0);
    _registers.set_flag_h(((val1 & 0xF) + (val2 & 0xF)) > 0xF);
    _registers.set_flag_c((res & 0x100) != 0);

    return (u8)res;
}

// ADD HL,n - Add n to HL.
// Sets flags appropriately.
void Cpu::_add_hl(const u16 val) {
    u16 hl = _registers.get_hl();
    int res = hl + val;

    _registers.set_flag_n(0);
    _registers.set_flag_h((hl & 0xFFF) + (val & 0xFFF) > 0xFFF);
    _registers.set_flag_c(res > 0xFFFF);

    _registers.set_hl((u16)res);
}

// Returns the logical AND of val1 with val2 and sets flags appropriately.
u8 Cpu::_and(const u8 val1, const u8 val2) {
    u8 res = val1 & val2;

    _registers.set_flag_z(res == 0);
    _registers.set_flag_n(0);
    _registers.set_flag_h(1);
    _registers.set_flag_c(0);

    return res;
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

// Returns the logical OR of val1 with val2 and sets flags appropriately.
u8 Cpu::_or(const u8 val1, const u8 val2) {
    u8 res = val1 | val2;

    _registers.set_flag_z(res == 0);
    _registers.set_flag_n(0);
    _registers.set_flag_h(0);
    _registers.set_flag_c(0);

    return res;
}

// For RLC n - Rotate n left; Old bit 7 into Carry flag.
// C <- [7 <- 0] <- [7]
// Sets flags appropriately.
u8 Cpu::_rlc(const u8 val) {
    const bool old_bit_7 = val >> 7;
    const u8 res = (val << 1) | old_bit_7;

    _registers.set_flag_z(res == 0);
    _registers.set_flag_n(0);
    _registers.set_flag_h(0);
    _registers.set_flag_c(old_bit_7);

    return res;
}

// For RL n - Rotate n left through Carry flag.
// C <- [7 <- 0] <- [C]
// Sets flags appropriately.
u8 Cpu::_rl(const u8 val) {
    const bool old_bit_7 = val >> 7;
    const u8 res = (val << 1) | _registers.get_flag_c();

    _registers.set_flag_z(res == 0);
    _registers.set_flag_n(0);
    _registers.set_flag_h(0);
    _registers.set_flag_c(old_bit_7);

    return res;
}

// For RR n - Rotate n right through Carry flag.
// C -> [7 -> 0] -> C
// Sets flags appropriately.
u8 Cpu::_rr(const u8 val) {
    bool old_flag_c = _registers.get_flag_c();
    _registers.set_flag_c(val & 1);

    u8 res = (old_flag_c << 7) | (val >> 1);

    _registers.set_flag_z(res == 0);
    _registers.set_flag_n(0);
    _registers.set_flag_h(0);
    
    return res;
}

// RST n - Push present address onto stack. Jump to address 0x0000 + n.
void Cpu::_rst(const u8 n) {
    _push_stack(_registers.get_pc());
    _registers.set_pc(n);
}

// Performs shift left to the given val (into carry flag), 
// and returns the result. 
// Also sets flags appropriately.
u8 Cpu::_sla(const u8 val) {
    _registers.set_flag_c(val >> 7);

    u8 res = val << 1;
    _registers.set_flag_z(res == 0);

    _registers.set_flag_n(0);
    _registers.set_flag_h(0);

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

// Returns the value of val1 - val2, setting flags appropriately.
// Used for SUB A,n
u8 Cpu::_sub(const u8 val1, const u8 val2) {
    u8 res = val1 - val2;

    _registers.set_flag_z(res == 0);
    _registers.set_flag_n(1);
    _registers.set_flag_h((val1 & 0xF) < (val2 & 0xF));
    _registers.set_flag_c(val1 < val2);

    return res;
}

// Swaps upper & lower nibbles of n and returns it. Sets flags appropriately. 
u8 Cpu::_swap(const u8 n) {
    u8 res = ((n & 0xF) << 4) | (n >> 4);

    _registers.set_flag_z(res == 0);
    _registers.set_flag_n(0);
    _registers.set_flag_h(0);
    _registers.set_flag_c(0);

    return res;
}
