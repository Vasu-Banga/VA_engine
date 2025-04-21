#include "Input.h"
#include "Helper.h"


std::unordered_map<SDL_Scancode, INPUT_STATE> Input::keyboard_states;
std::vector<SDL_Scancode> Input::just_became_down_scancodes;
std::vector<SDL_Scancode> Input::just_became_up_scancodes;

std::unordered_map<Uint8, INPUT_STATE> Input::mouse_button_states;
std::vector<Uint8> Input::just_became_down_buttons;
std::vector<Uint8> Input::just_became_up_buttons;

glm::vec2 Input::mouse_position;
float Input::mouse_scroll_this_frame = 0;

SDL_GameController* Input::controller = nullptr;
std::unordered_map<Uint8, INPUT_STATE> Input::controller_states;
std::vector<Uint8> Input::just_became_down_controller;
std::vector<Uint8> Input::just_became_up_controller;
glm::vec2 Input::controller_left_stick = {0.0f,0.0f};
glm::vec2 Input::controller_right_stick = {0.0f,0.0f};
float Input::controller_trigger_left = 0.0f;
float Input::controller_trigger_right = 0.0f;

const std::unordered_map<std::string, SDL_Scancode> __keycode_to_scancode = {
	// Directional (arrow) Keys
	{"up", SDL_SCANCODE_UP},
	{"down", SDL_SCANCODE_DOWN},
	{"right", SDL_SCANCODE_RIGHT},
	{"left", SDL_SCANCODE_LEFT},

	// Misc Keys
	{"escape", SDL_SCANCODE_ESCAPE},

	// Modifier Keys
	{"lshift", SDL_SCANCODE_LSHIFT},
	{"rshift", SDL_SCANCODE_RSHIFT},
	{"lctrl", SDL_SCANCODE_LCTRL},
	{"rctrl", SDL_SCANCODE_RCTRL},
	{"lalt", SDL_SCANCODE_LALT},
	{"ralt", SDL_SCANCODE_RALT},

	// Editing Keys
	{"tab", SDL_SCANCODE_TAB},
	{"return", SDL_SCANCODE_RETURN},
	{"enter", SDL_SCANCODE_RETURN},
	{"backspace", SDL_SCANCODE_BACKSPACE},
	{"delete", SDL_SCANCODE_DELETE},
	{"insert", SDL_SCANCODE_INSERT},

	// Character Keys
	{"space", SDL_SCANCODE_SPACE},
	{"a", SDL_SCANCODE_A},
	{"b", SDL_SCANCODE_B},
	{"c", SDL_SCANCODE_C},
	{"d", SDL_SCANCODE_D},
	{"e", SDL_SCANCODE_E},
	{"f", SDL_SCANCODE_F},
	{"g", SDL_SCANCODE_G},
	{"h", SDL_SCANCODE_H},
	{"i", SDL_SCANCODE_I},
	{"j", SDL_SCANCODE_J},
	{"k", SDL_SCANCODE_K},
	{"l", SDL_SCANCODE_L},
	{"m", SDL_SCANCODE_M},
	{"n", SDL_SCANCODE_N},
	{"o", SDL_SCANCODE_O},
	{"p", SDL_SCANCODE_P},
	{"q", SDL_SCANCODE_Q},
	{"r", SDL_SCANCODE_R},
	{"s", SDL_SCANCODE_S},
	{"t", SDL_SCANCODE_T},
	{"u", SDL_SCANCODE_U},
	{"v", SDL_SCANCODE_V},
	{"w", SDL_SCANCODE_W},
	{"x", SDL_SCANCODE_X},
	{"y", SDL_SCANCODE_Y},
	{"z", SDL_SCANCODE_Z},
	{"0", SDL_SCANCODE_0},
	{"1", SDL_SCANCODE_1},
	{"2", SDL_SCANCODE_2},
	{"3", SDL_SCANCODE_3},
	{"4", SDL_SCANCODE_4},
	{"5", SDL_SCANCODE_5},
	{"6", SDL_SCANCODE_6},
	{"7", SDL_SCANCODE_7},
	{"8", SDL_SCANCODE_8},
	{"9", SDL_SCANCODE_9},
	{"/", SDL_SCANCODE_SLASH},
	{";", SDL_SCANCODE_SEMICOLON},
	{"=", SDL_SCANCODE_EQUALS},
	{"-", SDL_SCANCODE_MINUS},
	{".", SDL_SCANCODE_PERIOD},
	{",", SDL_SCANCODE_COMMA},
	{"[", SDL_SCANCODE_LEFTBRACKET},
	{"]", SDL_SCANCODE_RIGHTBRACKET},
	{"\\", SDL_SCANCODE_BACKSLASH},
	{"'", SDL_SCANCODE_APOSTROPHE}
};

void Input::Init() {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
	keyboard_states.clear();
	just_became_down_scancodes.clear();
	just_became_up_scancodes.clear();
    mouse_button_states.clear();
    just_became_down_buttons.clear();
    just_became_up_buttons.clear();
	
	for (int i = 0; i < SDL_NumJoysticks(); i++) {
		if (SDL_IsGameController(i)) {
			controller = SDL_GameControllerOpen(i);
			if (controller) {
				break;
			}
		}
	}
}

void Input::ProcessEvent(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        SDL_Scancode code = e.key.keysym.scancode;
        keyboard_states[code] = INPUT_STATE_JUST_BECAME_DOWN;
        just_became_down_scancodes.push_back(code);
    }
    else if (e.type == SDL_KEYUP) {
        SDL_Scancode code = e.key.keysym.scancode;
        keyboard_states[code] = INPUT_STATE_JUST_BECAME_UP;
        just_became_up_scancodes.push_back(code);
    }
    else if (e.type == SDL_MOUSEBUTTONDOWN) {
        Uint8 button = e.button.button;
        mouse_button_states[button] = INPUT_STATE_JUST_BECAME_DOWN;
        just_became_down_buttons.push_back(button);
    }
    else if (e.type == SDL_MOUSEBUTTONUP) {
        Uint8 button = e.button.button;
        mouse_button_states[button] = INPUT_STATE_JUST_BECAME_UP;
        just_became_up_buttons.push_back(button);
    }
    else if (e.type == SDL_MOUSEWHEEL) {
        mouse_scroll_this_frame += e.wheel.preciseY;
    }
    else if (e.type == SDL_MOUSEMOTION) {
        mouse_position = { e.motion.x, e.motion.y };
	}
	else if (e.type == SDL_CONTROLLERBUTTONDOWN) {
		Uint8 button = e.cbutton.button;
		std::cout << "Button " << static_cast<int>(button) << " pressed " << std::endl;
		controller_states[button] = INPUT_STATE_JUST_BECAME_DOWN;
		just_became_down_controller.push_back(button);
	}
	else if (e.type == SDL_CONTROLLERBUTTONUP) {
		Uint8 button = e.cbutton.button;
		controller_states[button] = INPUT_STATE_JUST_BECAME_UP;
		just_became_up_controller.push_back(button);
	}
	else if (e.type == SDL_CONTROLLERAXISMOTION) {
		auto NormalizeAxis = [](Sint16 value) -> float {
			const float dead_zone = 0000.0f; // optional: to ignore small accidental input
			if (std::abs(value) == 1.0f) return value;
			if (std::abs(value) < dead_zone) return 0.0f;

			// Normalize to range [-1.0, 1.0]
			float normalized = value < 0
				? value / 32768.0f
				: value / 32767.0f;

			// Clamp just in case
			return std::clamp(normalized, -1.0f, 1.0f);
		};

		if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
			controller_left_stick.x = NormalizeAxis(e.caxis.value);
		}
		if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
			controller_left_stick.y = NormalizeAxis(e.caxis.value);
		}
		if (e.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX) {
			controller_right_stick.x = NormalizeAxis(e.caxis.value);
		}
		if (e.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY) {
			controller_right_stick.y = NormalizeAxis(e.caxis.value);
		}
		if (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) {
			controller_trigger_left = e.caxis.value / 32767.0f;
		}
		if (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
			controller_trigger_right = e.caxis.value / 32767.0f;
		}
	}
}

void Input::LateUpdate() {
    for (SDL_Scancode code : just_became_down_scancodes) {
        keyboard_states[code] = INPUT_STATE_DOWN;
    }
    just_became_down_scancodes.clear();

    for (SDL_Scancode code : just_became_up_scancodes) {
        keyboard_states[code] = INPUT_STATE_UP;
    }
    just_became_up_scancodes.clear();
    for (Uint8 button : just_became_down_buttons) {
        mouse_button_states[button] = INPUT_STATE_DOWN;
    }
    just_became_down_buttons.clear();

    for (Uint8 button : just_became_up_buttons) {
        mouse_button_states[button] = INPUT_STATE_UP;
    }
    just_became_up_buttons.clear();

	for (Uint8 button : just_became_up_controller) {
		controller_states[button] = INPUT_STATE_UP;
	}
	just_became_up_controller.clear();

	for (Uint8 button : just_became_down_controller) {
		controller_states[button] = INPUT_STATE_DOWN;
	}
	just_became_down_controller.clear();

    mouse_scroll_this_frame = 0.0f;
}

bool Input::GetKey(std::string keycode) {
	if (__keycode_to_scancode.count(keycode) == 0) return false;
    auto it = keyboard_states.find((__keycode_to_scancode.find(keycode)->second));
    if (it != keyboard_states.end()) {
        return (it->second == INPUT_STATE_DOWN || it->second == INPUT_STATE_JUST_BECAME_DOWN);
    }
    return false;
}

bool Input::GetKeyDown(std::string keycode) {
	if (__keycode_to_scancode.count(keycode) == 0) return false;
	auto it = keyboard_states.find((__keycode_to_scancode.find(keycode)->second));
    return (it != keyboard_states.end() && it->second == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetKeyUp(std::string keycode) {
	if (__keycode_to_scancode.count(keycode) == 0) return false;
	auto it = keyboard_states.find((__keycode_to_scancode.find(keycode)->second));
    return (it != keyboard_states.end() && it->second == INPUT_STATE_JUST_BECAME_UP);
}

bool Input::GetMouseButton(int button) {
    auto it = mouse_button_states.find(button);
    return (it != mouse_button_states.end() &&
        (it->second == INPUT_STATE_DOWN || it->second == INPUT_STATE_JUST_BECAME_DOWN));
}

bool Input::GetMouseDown(int button) {
    auto it = mouse_button_states.find(button);
    return (it != mouse_button_states.end() && it->second == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetMouseUp(int button) {
    auto it = mouse_button_states.find(button);
    return (it != mouse_button_states.end() && it->second == INPUT_STATE_JUST_BECAME_UP);
}

glm::vec2 Input::GetMousePosition() {
    return mouse_position;
}

float Input::GetMouseScrollDelta() {
    return mouse_scroll_this_frame;
}

void Input::HideCursor() {
    SDL_ShowCursor(SDL_DISABLE);
}
void Input::ShowCursor() {
    SDL_ShowCursor(SDL_ENABLE);

}

bool Input::GetControllerButton(int button) {
	auto it = controller_states.find(button);
	return (it != controller_states.end() &&
		(it->second == INPUT_STATE_DOWN || it->second == INPUT_STATE_JUST_BECAME_DOWN));
}

bool Input::GetControllerDown(int button) {
	auto it = controller_states.find(static_cast<Uint8>(button));
	return (it != controller_states.end() && it->second == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetControllerUp(int button) {
	auto it = controller_states.find(button);
	return (it != controller_states.end() && it->second == INPUT_STATE_JUST_BECAME_UP);
}

glm::vec2 Input::GetControllerLeftStick() {
	return controller_left_stick;
}

glm::vec2 Input::GetControllerRightStick() {
	return controller_right_stick;
}

float Input::GetControllerLeftTrigger() {
	return controller_trigger_left;
}

float Input::GetControllerRightTrigger() {
	return controller_trigger_right;
}