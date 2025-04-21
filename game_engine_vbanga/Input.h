#ifndef INPUT_H
#define INPUT_H

#include "SDL2/SDL.h"
#include <unordered_map>
#include <vector>
#include <string>
#include "glm/glm.hpp"

enum INPUT_STATE { INPUT_STATE_UP, INPUT_STATE_JUST_BECAME_DOWN, INPUT_STATE_DOWN, INPUT_STATE_JUST_BECAME_UP };

class Input
{
public:
	static void Init(); // Call before main loop begins.
	static void ProcessEvent(const SDL_Event& e); // Call every frame at start of event loop.
	static void LateUpdate();

	static bool GetKey(std::string keycode);
	static bool GetKeyDown(std::string keycode);
	static bool GetKeyUp(std::string keycode);

	static bool GetMouseButton(int button);
	static bool GetMouseDown(int button);
	static bool GetMouseUp(int button);
	static glm::vec2 GetMousePosition();
	static float GetMouseScrollDelta();

	static void HideCursor();
	static void ShowCursor();


	static bool GetControllerButton(int button);
	static bool GetControllerDown(int button);
	static bool GetControllerUp(int button);
	static glm::vec2 GetControllerLeftStick();
	static glm::vec2 GetControllerRightStick();
	static float GetControllerLeftTrigger();
	static float GetControllerRightTrigger();

private:
	static std::unordered_map<SDL_Scancode, INPUT_STATE> keyboard_states;
	static std::vector<SDL_Scancode> just_became_down_scancodes;
	static std::vector<SDL_Scancode> just_became_up_scancodes;
	static std::unordered_map<Uint8, INPUT_STATE> mouse_button_states;
	static std::vector<Uint8> just_became_down_buttons;
	static std::vector<Uint8> just_became_up_buttons;
	static glm::vec2 mouse_position;
	static float mouse_scroll_this_frame;

	static SDL_GameController* controller;
	static std::unordered_map<Uint8, INPUT_STATE> controller_states;
	static std::vector<Uint8> just_became_down_controller;
	static std::vector<Uint8> just_became_up_controller;
	static glm::vec2 controller_left_stick;
	static glm::vec2 controller_right_stick;
	static float controller_trigger_left;
	static float controller_trigger_right;
};

#endif