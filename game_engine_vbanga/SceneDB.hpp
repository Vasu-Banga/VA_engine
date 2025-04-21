//
//  SceneDB.hpp
//  game_engine
//
//  Created by Vasu Banga on 2/2/25.
//

#ifndef SceneDB_hpp
#define SceneDB_hpp

#include <stdio.h>
#include "glm/glm.hpp"
#include "include/rapidjson/document.h"
#include <vector>
#include <queue>
#include <string>
#include <filesystem>
#include <unordered_set>
#include "ActorDB.h"
#include <set>

void ReadJsonFile(const std::string& path, rapidjson::Document& out_document);

//struct Actor
//{
//public:
//    std::string actor_name = "";
//    char view = '?';
//    glm::ivec2 position = glm::ivec2(0,0);
//    glm::ivec2 velocity = glm::ivec2(0,0);
//    bool blocking = false;
//    std::string nearby_dialogue = "";
//    std::string contact_dialogue = "";
//    bool tagged = false;
//
//    Actor(std::string actor_name, char view, glm::ivec2 position, glm::ivec2 initial_velocity,
//        bool blocking, std::string nearby_dialogue, std::string contact_dialogue)
//        : actor_name(actor_name), view(view), position(position), velocity(initial_velocity), blocking(blocking), nearby_dialogue(nearby_dialogue), contact_dialogue(contact_dialogue) {}
//
//    Actor() {}
//};

class SceneDB {
private:
	rapidjson::Document currentScene;
	std::unordered_map<int, ActorDB*> sceneActors;
	std::unordered_map<std::string, ActorDB*> templates;
	std::string sceneName;
	std::string nextScene;
	int width = 13;
	int height = 9;
	int numActors;
	static lua_State* lua_state;
	std::unordered_set<std::string> loadedComponents;
	static SceneDB* currentInstance;
	static SDL_Renderer* renderer;
	std::vector<ActorDB*> actors_to_add;
	std::vector<int> actors_to_remove;

	static std::unordered_map<std::string, std::vector<std::pair<luabridge::LuaRef, luabridge::LuaRef>>> eventSubscriptions;
	static std::vector<std::tuple<std::string, luabridge::LuaRef, luabridge::LuaRef>> toSubscribe;
	static std::vector<std::tuple<std::string, luabridge::LuaRef, luabridge::LuaRef>> toUnsubscribe;


public:
	void loadScene(std::string, const bool&);

	//void updateActors();
	//void setMovement(const char&);
	//glm::ivec2 getLocation();
	void update();
	//void printMap(int&, int&, bool&, bool&, bool&);
	SceneDB(const int&, const int&);
	~SceneDB();
	void loadValues(const rapidjson::Value& values, ActorDB* tempActor, bool);
	void EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table);
	void loadComponents();
	static void log(std::string);
	void loadTemplate(const std::string&, ActorDB*);
	static void setLuaState(lua_State*);
	static luabridge::LuaRef Find(std::string name);
	static luabridge::LuaRef FindAll(std::string name);
	void lateUpdate();
	void start();
	static void quit();
	static void sleep(int);
	static void openURL(std::string);
	static luabridge::LuaRef Instantiate(std::string templateName);
	static void Destroy(luabridge::LuaRef);
	void alterActors();
	static void DontDestroy(luabridge::LuaRef);
	void checkForChange();
	static void Load(std::string value) { currentInstance->nextScene = value; }
	static std::string getCurrent() { return currentInstance->sceneName; }
	static void EventPublish(std::string type, luabridge::LuaRef eventObject);
	static void EventSubscribe(std::string type, luabridge::LuaRef component, luabridge::LuaRef func);
	static void EventUnsubscribe(std::string type, luabridge::LuaRef component, luabridge::LuaRef func);

};

#endif /* SceneDB_hpp */
