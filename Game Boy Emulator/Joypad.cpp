#include <iostream>

#include "Joypad.h"
#include "utils.h"


using namespace std;


Joypad::Joypad(Memory& memory) : _memory(memory) {
	_is_button_right_pressed = false;
	_is_button_left_pressed = false;
	_is_button_up_pressed = false;
	_is_button_down_pressed = false;
	_is_button_a_pressed = false;
	_is_button_b_pressed = false;
	_is_button_select_pressed = false;
	_is_button_start_pressed = false;
}


// Updates bits 0-3 in the joypad input register (FF00 - JOYP) based on joypad input.
void Joypad::update_joyp_register() {
	const u8 joyp_val = _memory.read(0xFF00);

	// Set the lower nibble of the register value (to 1=unpressed), since we'll update it
	// if any button was pressed
	u8 new_joyp_val = joyp_val | 0xF;

	if (get_bit(joyp_val, 4) == 0) {		// Case in which direction buttons are enabled
		if (_is_button_right_pressed) {
			new_joyp_val = set_bit(new_joyp_val, 0, 0);
		}
		if (_is_button_left_pressed) {
			new_joyp_val = set_bit(new_joyp_val, 1, 0);
		}
		if (_is_button_up_pressed) {
			new_joyp_val = set_bit(new_joyp_val, 2, 0);
		}
		if (_is_button_down_pressed) {
			new_joyp_val = set_bit(new_joyp_val, 3, 0);
		}
	}
	if (get_bit(joyp_val, 5) == 0) {		// Case in which action buttons are enabled
		if (_is_button_a_pressed) {
			new_joyp_val = set_bit(new_joyp_val, 0, 0);
		}
		if (_is_button_b_pressed) {
			new_joyp_val = set_bit(new_joyp_val, 1, 0);
		}
		if (_is_button_select_pressed) {
			new_joyp_val = set_bit(new_joyp_val, 2, 0);
		}
		if (_is_button_start_pressed) {
			new_joyp_val = set_bit(new_joyp_val, 3, 0);
		}
	}

	// Update JOYP
	_memory.write(0xFF00, new_joyp_val);
}

void Joypad::on_button_pressed(Button button) {
	cout << "Button was pressed" << endl;

	switch (button) {
	case Button::right:
		_is_button_right_pressed = true;
		break;
	case Button::left:
		_is_button_left_pressed = true;
		break;
	case Button::up:
		_is_button_up_pressed = true;
		break;
	case Button::down:
		_is_button_down_pressed = true;
		break;
	case Button::a:
		_is_button_a_pressed = true;
		break;
	case Button::b:
		_is_button_b_pressed = true;
		break;
	case Button::select:
		_is_button_select_pressed = true;
		break;
	case Button::start:
		_is_button_start_pressed = true;
		break;
	}
}

void Joypad::on_button_released(Button button) {
	cout << "Button was released" << endl;

	switch (button) {
	case Button::right:
		_is_button_right_pressed = false;
		break;
	case Button::left:
		_is_button_left_pressed = false;
		break;
	case Button::up:
		_is_button_up_pressed = false;
		break;
	case Button::down:
		_is_button_down_pressed = false;
		break;
	case Button::a:
		_is_button_a_pressed = false;
		break;
	case Button::b:
		_is_button_b_pressed = false;
		break;
	case Button::select:
		_is_button_select_pressed = false;
		break;
	case Button::start:
		_is_button_start_pressed = false;
		break;
	}
}
