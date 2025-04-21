//
//  main.cpp
//  game_engine
//
//  Created by Vasu Banga on 1/26/25.
//

#include <stdio.h>
#include <iostream>
#include <vector>
#include "SceneDB.hpp"
#include "SDL.h"
#include "Helper.h"
#include "ImageDB.h"
#include "AudioHelper.h"
#include "glm/glm.hpp"
#include "include/rapidjson/document.h"
#include "include/rapidjson/filereadstream.h"
#include <filesystem>
#include <queue>
#include "SDL_ttf.h"
#include "TextDB.h"
#include "AudioDB.h"
#include "Input.h"
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "box2d/box2d.h"

int main(int argc, char* argv[]) {

	//Check for a resources directory

	const std::string resourcesDir{ "resources" };
	if (!(std::filesystem::exists(resourcesDir) && std::filesystem::is_directory(resourcesDir))) {
		std::cout << "error: resources/ missing";
		exit(0);
	}

	const std::string configFile{ "resources/game.config" };
	if (!(std::filesystem::exists(configFile))) {
		std::cout << "error: resources/game.config missing";
		exit(0);
	}

	rapidjson::Document config;

	ReadJsonFile(configFile, config);

	rapidjson::Document rendering;
	bool renderingExists = false;
	if (std::filesystem::exists("resources/rendering.config")) {
		ReadJsonFile("resources/rendering.config", rendering);
		renderingExists = true;
	}

	TTF_Init();
	Input::Init();

	int x_resolution = 640;
	int y_resolution = 360;
	int render_red = 255;
	int render_blue = 255;
	int render_green = 255;
	std::string game_title = "";
	std::string initialScene;




	if (renderingExists && rendering.HasMember("x_resolution")) {
		x_resolution = rendering["x_resolution"].GetInt();
	}
	if (renderingExists && rendering.HasMember("y_resolution")) {
		y_resolution = rendering["y_resolution"].GetInt();
	}
	if (renderingExists && rendering.HasMember("zoom_factor")) {
		ActorDB::setZoomFactor(rendering["zoom_factor"].GetFloat());
	}
	else {
		ActorDB::setZoomFactor(1.0f);
	}
	if (renderingExists && rendering.HasMember("clear_color_r")) {
		render_red = rendering["clear_color_r"].GetInt();
	}
	if (renderingExists && rendering.HasMember("clear_color_g")) {
		render_green = rendering["clear_color_g"].GetInt();
	}
	if (renderingExists && rendering.HasMember("clear_color_b")) {
		render_blue = rendering["clear_color_b"].GetInt();
	}
	if (config.HasMember("game_title")) {
		game_title = config["game_title"].GetString();
	}
	if (config.HasMember("initial_scene")) {
		initialScene = config["initial_scene"].GetString();
	}


	SDL_Window* window = Helper::SDL_CreateWindow(game_title.c_str(), 50, 100, x_resolution, y_resolution, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = Helper::SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	lua_State* lua_state = luaL_newstate();
	luaL_openlibs(lua_state);
	ImageDB::setWidth(x_resolution);
	ImageDB::setHeight(y_resolution);

	//TODO: do the same thing for renderer, will save a lot of function processing. Will take a while though, so do this later
	SceneDB::setLuaState(lua_state);
	ActorDB::setLuaState(lua_state);
	TextDB::setRenderer(renderer);
	ImageDB::setRenderer(renderer);

	//Time to check the beginning images, if they exist

	SceneDB sceneManager(x_resolution, y_resolution);

	SDL_SetRenderDrawColor(renderer, render_red, render_green, render_blue, 255);
	SDL_RenderClear(renderer);

	bool keepLooping = true;
	//Intro loop



	sceneManager.loadComponents();
	if (initialScene == "") {
		std::cout << "No initial scene defined" << std::endl;
		exit(1);
	}
	sceneManager.loadScene(initialScene,true);
	while (keepLooping) {

		bool skipFrame = false;
		SDL_Event next_event;
		while (Helper::SDL_PollEvent(&next_event)) {
			if (next_event.type == SDL_QUIT) {
				keepLooping = false;
				//Helper::SDL_RenderPresent(renderer);
				//exit(0);
			}
			Input::ProcessEvent(next_event);
		}

		if (skipFrame) {
			continue;
		}



		//Render everything!
		SDL_SetRenderDrawColor(renderer, render_red, render_green, render_blue, 255);
		SDL_RenderClear(renderer);
		// Now we can do a lot of things
		sceneManager.start();
		sceneManager.update();
		sceneManager.lateUpdate();
		sceneManager.alterActors();
		ImageDB::RenderAll();
		Input::LateUpdate();
		RigidBody::step();
		sceneManager.checkForChange();
		Helper::SDL_RenderPresent(renderer);

	}
	return 0;
}
