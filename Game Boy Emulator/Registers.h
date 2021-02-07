#pragma once

#include "types.h"


class Registers {
public:
	Registers();

	// Accessors
	u8 get_a() const;
	u8 get_b() const;
	u8 get_c() const;
	u8 get_d() const;
	u8 get_e() const;
	u8 get_h() const;
	u8 get_l() const;

	u8 get_f() const;

	bool get_flag_z() const;
	bool get_flag_n() const;
	bool get_flag_h() const;
	bool get_flag_c() const;

	u16 get_af() const;
	u16 get_bc() const;
	u16 get_de() const;
	u16 get_hl() const;

	u16 get_sp() const;
	u16 get_pc() const;

	// Modifiers
	void set_a(const u8 val);
	void set_b(const u8 val);
	void set_c(const u8 val);
	void set_d(const u8 val);
	void set_e(const u8 val);
	void set_h(const u8 val);
	void set_l(const u8 val);

	void set_f(const u8 val);

	void set_flag_z();
	void clear_flag_z();
	void set_flag_n();
	void clear_flag_n();
	void set_flag_h();
	void clear_flag_h();
	void set_flag_c();
	void clear_flag_c();

	void set_af(const u16 val);
	void set_bc(const u16 val);
	void set_de(const u16 val);
	void set_hl(const u16 val);

	void set_sp(const u16 val);
	void inc_sp();
	void dec_sp();
	void set_pc(const u16 val);
	void inc_pc();


private:
	u8 _a;
	u8 _b;
	u8 _c;
	u8 _d;
	u8 _e;
	u8 _h;
	u8 _l;
	u8 _f;	// Flags
	
	u16 _sp;
	u16 _pc;
};