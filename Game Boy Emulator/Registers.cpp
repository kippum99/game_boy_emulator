#include "Registers.h"

#include "utils.h"


Registers::Registers() {
	_a = 0;
	_b = 0;
	_c = 0;
	_d = 0;
	_e = 0;
	_h = 0;
	_l = 0;
	_f = 0;
	_sp = 0;
	_pc = 0;
}


// Accessor definitions

u8 Registers::get_a() const {
	return _a;
}

u8 Registers::get_b() const {
	return _b;
}

u8 Registers::get_c() const {
	return _c;
}

u8 Registers::get_d() const {
	return _d;
}

u8 Registers::get_e() const {
	return _e;
}

u8 Registers::get_h() const {
	return _h;
}

u8 Registers::get_l() const {
	return _l;
}


u8 Registers::get_f() const {
	return _f;
}


bool Registers::get_flag_z() const {
	return _f >> 7;
}

bool Registers::get_flag_n() const {
	return (_f >> 6) & 1;
}

bool Registers::get_flag_h() const {
	return (_f >> 5) & 1;
}

bool Registers::get_flag_c() const {
	return (_f >> 4) & 1;
}


u16 Registers::get_af() const {
	return _a << 8 | _f;
}

u16 Registers::get_bc() const {
	return _b << 8 | _c;
}

u16 Registers::get_de() const {
	return _d << 8 | _e;
}

u16 Registers::get_hl() const {
	return _h << 8 | _l;
}

u16 Registers::get_pc() const {
	return _pc;
}

u16 Registers::get_sp() const {
	return _sp;
}


// Modifier definitions

void Registers::set_a(const u8 val) {
	_a = val;
}

void Registers::set_b(const u8 val) {
	_b = val;
}

void Registers::set_c(const u8 val) {
	_c = val;
}

void Registers::set_d(const u8 val) {
	_d = val;
}

void Registers::set_e(const u8 val) {
	_e = val;
}

void Registers::set_h(const u8 val) {
	_h = val;
}

void Registers::set_l(const u8 val) {
	_l = val;
}


void Registers::set_flag_z(bool val) {
	_f = set_bit(_f, 7, val);
}

void Registers::set_flag_n(bool val) {
	_f = set_bit(_f, 6, val);
}

void Registers::set_flag_h(bool val) {
	_f = set_bit(_f, 5, val);
}

void Registers::set_flag_c(bool val) {
	_f = set_bit(_f, 4, val);
}


void Registers::set_af(const u16 val) {
	_a = val >> 8;
	_f = val & 0xFF;
}


void Registers::set_bc(const u16 val) {
	_b = val >> 8;
	_c = val & 0xFF;
}

void Registers::inc_bc() {
	set_bc(get_bc() + 1);
}

void Registers::dec_bc() {
	set_bc(get_bc() - 1);
}


void Registers::set_de(const u16 val) {
	_d = val >> 8;
	_e = val & 0xFF;
}

void Registers::inc_de() {
	set_de(get_de() + 1);
}

void Registers::dec_de() {
	set_de(get_de() - 1);
}


void Registers::set_hl(const u16 val) {
	_h = val >> 8;
	_l = val & 0xFF;
}

void Registers::inc_hl() {
	set_hl(get_hl() + 1);
}

void Registers::dec_hl() {
	set_hl(get_hl() - 1);
}


void Registers::set_sp(const u16 val) {
	_sp = val;
}

void Registers::inc_sp() {
	_sp++;
}

void Registers::dec_sp() {
	_sp--;
}


void Registers::set_pc(const u16 val) {
	_pc = val;
}

void Registers::inc_pc() {
	_pc++;
}