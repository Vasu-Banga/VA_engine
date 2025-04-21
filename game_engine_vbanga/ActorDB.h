#pragma once
#include "glm/glm.hpp"
#include "SDL_image.h"
#include <string>
#include <filesystem>
#include <iostream>
#include <optional>
#include <unordered_set>
#include "AudioDB.h"
#include "ImageDB.h"
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "RigidBody.h"
#include "ParticleSystem.h"

class ActorDB
{
private:
	size_t key;
	std::string actor_name;
	static float zoomFactor;
	std::map<std::string, luabridge::LuaRef> components;
	static lua_State* lua_state;
	static int componentCount;
	std::vector<std::pair<std::string, luabridge::LuaRef>> components_to_add;
	std::vector<std::string> components_to_remove;
	std::unordered_set<std::string> startComponents;
	bool run = true;
	bool toDelete = false;
	bool persistent = false;
	RigidBody* body = nullptr;
	ParticleSystem* particle = nullptr;
	std::string bodyKey;
	std::string particleKey;
	/*bool collider = false;
	bool trigger = false;*/


public:
	ActorDB(int tempKey) : key(tempKey) {}
	ActorDB(){}
	void setName(std::string);
	int getKey();
	std::string getName();
	void start();
	void update();
	void lateUpdate();
	void setKey(int);
	static float getZoomFactor();
	static void setDims(int width, int height);
	static void setCamOffsetX(float val);
	static void setCamOffsetY(float val);
	static void setZoomFactor(float);
	static void setFlipOnMove(bool);
	void addComponent(const std::string& key, luabridge::LuaRef value);
	void setStart(std::string val);
	void updateTemplates();
	void EstablishInheritance(luabridge::LuaRef& instance_table, const luabridge::LuaRef& parent_table);
	std::optional<luabridge::LuaRef*> componentExists(const std::string&);
	luabridge::LuaRef getComponentByKey(std::string key);
	luabridge::LuaRef getComponent(std::string type);
	luabridge::LuaRef getComponents(std::string type);
	static void setLuaState(lua_State*);
	void alterContainer();
	luabridge::LuaRef AddComponent(std::string);
	void RemoveComponent(luabridge::LuaRef);
	void disableAll();
	bool getRun() { return run;  }
	void setRun(bool val) { run = val; }
	void setDelete(bool val) { toDelete = val; }
	bool getDelete() { return toDelete; }
	void setPersistence(bool val) { persistent = val; }
	bool getPersistence() { return persistent; }
	void setRigidBody(RigidBody* value, std::string key) { body = value; bodyKey = key; }
	void setParticleSystem(ParticleSystem* value, std::string key) { particle = value; particleKey = key; }
	RigidBody* getRigidBody(){ return body;}
	/*void setCollider(bool val) { collider = val; }
	bool getCollider() { return collider; }
	void setTrigger(bool val) { trigger = val; }
	bool getTrigger() { return trigger; }*/

	void Delete();


	std::map<std::string, luabridge::LuaRef>& getComponentsMap() { return components; }
	static lua_State* getLuaState() { return lua_state; }
	

};

