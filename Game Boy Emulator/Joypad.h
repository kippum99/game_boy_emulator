#pragma once

#include "Button.h"
#include "Memory.h"


class Joypad {
public:
	Joypad(Memory& memory);

	void update_joyp_register();
	void on_button_pressed(Button button);
	void on_button_released(Button button);

private:
	Memory& _memory;

	bool _is_button_right_pressed;
	bool _is_button_left_pressed;
	bool _is_button_up_pressed;
	bool _is_button_down_pressed;
	bool _is_button_a_pressed;
	bool _is_button_b_pressed;
	bool _is_button_select_pressed;
	bool _is_button_start_pressed;
};
