#include "ActorDB.h"
#include "Helper.h"





void ReportError(const std::string& actor_name, const luabridge::LuaException& e)
{
	std::string error_message = e.what();

	/* Normalize file paths across platforms */
	std::replace(error_message.begin(), error_message.end(), '\\', '/');

	/* Display (with color codes) */
	std::cout << "\033[31m" << actor_name << " : " << error_message << "\033[0m" << std::endl;
}



void ActorDB::setName(std::string name)
{
	actor_name = name;
}

void ActorDB::start() {

	for (const auto& [key, component] : components) {
		if (component["enabled"].isBool() && !component["enabled"]) continue;
		if (!run || startComponents.count(key) != 0) {
			if (component["OnStart"].isFunction()) {
				try {
					component["OnStart"](component);
				}
				catch (luabridge::LuaException const& e) {
					ReportError(actor_name, e);
				}
			}
			else if (key == bodyKey) {
				body->onStart();
			}
			else if (key == particleKey) {
				particle->onStart();
			}
		}
	}
	run = true;
	startComponents.clear();
	//Now we run our checks
}

void ActorDB::update() {
	for (const auto& [key, component] : components) {
		if (component["enabled"].isBool() && !component["enabled"]) continue;
		if (component["OnUpdate"].isFunction()) {
			try {
				component["OnUpdate"](component);
			}
			catch (luabridge::LuaException const& e) {
				ReportError(actor_name, e);
			}
		}
	}
}

void ActorDB::lateUpdate() {
	for (const auto& [key, component] : components) {
		if (component["enabled"].isBool() && !component["enabled"]) continue;
		if (component["OnLateUpdate"].isFunction()) {
			try {
				component["OnLateUpdate"](component);
			}
			catch (luabridge::LuaException const& e) {
				ReportError(actor_name, e);
			}
		}
		else if (key == bodyKey) {
			body->lateUpdate();
		}
	}
}

void ActorDB::Delete() {
	for (const auto& [key, component] : components) {
		if (key == bodyKey) { body->OnDestroy(); delete body; body = nullptr; continue; }
		if (key == particleKey) {  delete particle; particle = nullptr; continue; }
		if (component["OnDestroy"].isFunction()) {
			try {
				component["OnDestroy"](component);
			}
			catch (luabridge::LuaException const& e) {
				ReportError(actor_name, e);
			}
		}
	}
}

void ActorDB::EstablishInheritance(luabridge::LuaRef& instance_table, const luabridge::LuaRef& parent_table)
{
	// Create a new metatable to define inheritance in Lua
	luabridge::LuaRef new_metatable = luabridge::newTable(lua_state);
	new_metatable["__index"] = parent_table;

	// Using Lua C-API (the raw lua stack):
	instance_table.push(lua_state);        // Push instance table to stack
	new_metatable.push(lua_state);         // Push the new metatable
	lua_setmetatable(lua_state, -2);       // Assign the metatable to the instance table
	lua_pop(lua_state, 1);                 // Pop instance table from stack
}

void ActorDB::updateTemplates() {
	//Go through each template, and fix that shi
	if (components.empty()) return;

	for (auto& [key, componentTemplate] : components) {
		//luabridge::LuaRef prevVal = component;
		if (componentTemplate["type"].cast<std::string>() == "RigidBody") continue;

		luabridge::LuaRef componentInstance = luabridge::newTable(lua_state);

		EstablishInheritance(componentInstance, componentTemplate);

		componentInstance["actor"] = this;
		componentInstance["key"] = key;

	}
}


int ActorDB::getKey() {
	return key;
}

std::string ActorDB::getName() {
	return actor_name;
}


float ActorDB::zoomFactor;

void ActorDB::setZoomFactor(float val) {
	zoomFactor = val;
}

float ActorDB::getZoomFactor() {
	return zoomFactor;
}

void ActorDB::setKey(int val) {
	key = val;
}

void ActorDB::addComponent(const std::string& key, luabridge::LuaRef value) {
	if (components.count(key) != 0) {
		auto it = components.find(key);
		it->second = value;
		return;
	}
	startComponents.insert(key);
	components.insert({ key,value });
}

std::optional<luabridge::LuaRef*> ActorDB::componentExists(const std::string& key) {
	if (components.count(key) == 0) return std::nullopt;
	return &components.find(key)->second;
}

luabridge::LuaRef ActorDB::getComponentByKey(std::string key) {
	auto it = components.find(key);
	if (it != components.end()) {
		return it->second;
	}
	return luabridge::LuaRef(lua_state);  // returns nil
}

luabridge::LuaRef ActorDB::getComponent(std::string type) {
	if (type == "Rigidbody") {
		if (body) {
			return components.find(bodyKey)->second;
		}
	}
	if (type == "ParticleSystem") {
		if (particle) {
			return components.find(particleKey)->second;
		}
	}
	for (const auto& [key, component] : components) {
		if (component["type"].isString() && component["type"] == type && component["enabled"]) {
			return component;
		}
	}
	return luabridge::LuaRef(lua_state);  // returns nil
}

luabridge::LuaRef ActorDB::getComponents(std::string type) {
	luabridge::LuaRef results = luabridge::newTable(lua_state);
	int index = 1;  // Lua tables start indexing at 1
	for (const auto& [key, component] : components) {
		if (component["type"].isString() && component["type"].cast<std::string>() == type) {
			results[index++] = component;
		}
	}
	return results;  // empty table if none found
}

void ActorDB::alterContainer() {
	//Smth smth smth smth smth
	for (std::pair<std::string, luabridge::LuaRef>& value : components_to_add) {
		components.insert(value);
	}
	components_to_add.clear();
	for (std::string& value : components_to_remove) {
		luabridge::LuaRef toBeDeleted = components.find(value)->second;
		if (value == bodyKey) { body->OnDestroy(); components.erase(value); delete body; body = nullptr; continue; }
		if (value == particleKey) { components.erase(value); delete particle; particle = nullptr; continue; }
		if (toBeDeleted["OnDestroy"].isFunction()) {
			try {
				toBeDeleted["OnDestroy"](toBeDeleted);
			}
			catch (luabridge::LuaException const& e) {
				ReportError(actor_name, e);
			}
		}
		components.erase(value);
	}
	components_to_remove.clear();
	//Now update your thingy if it ran
}


lua_State* ActorDB::lua_state;

void ActorDB::setLuaState(lua_State* val) {
	lua_state = val;
}


int ActorDB::componentCount = 0;


luabridge::LuaRef ActorDB::AddComponent(std::string type_name) {
	//so we're gonna load in the component, and then load in the thingy
	//I forgot how to add a component to an actor, one second
	luabridge::LuaRef componentTemplate = luabridge::getGlobal(lua_state, type_name.c_str());
	if (!componentTemplate.isTable() && type_name != "Rigidbody" && type_name != "ParticleSystem") {
		std::cout << "error: failed to locate component " << type_name;
		return luabridge::LuaRef(lua_state);
	}
	luabridge::LuaRef componentInstance = luabridge::newTable(lua_state);
	std::string componentKey = "r" + std::to_string(componentCount++);

	if (type_name == "Rigidbody") {
		//TODO THIS
		RigidBody* newVal = new RigidBody();
		setRigidBody(newVal, componentKey);
		componentInstance = luabridge::LuaRef(lua_state, newVal);
		newVal->setActor(this);
	}
	if (type_name == "ParticleSystem") {
		ParticleSystem* newVal = new ParticleSystem();
		setParticleSystem(newVal, componentKey);
		componentInstance = luabridge::LuaRef(lua_state, newVal);
	}
	else {
		EstablishInheritance(componentInstance, componentTemplate);


		componentInstance["key"] = componentKey;
		componentInstance["type"] = type_name;
		componentInstance["enabled"] = true;
		componentInstance["actor"] = luabridge::LuaRef(lua_state, this);
	}
	//components.insert({ componentKey, componentInstance });
	components_to_add.push_back({ componentKey, componentInstance });
	startComponents.insert(componentKey);
	return componentInstance;
}

void ActorDB::RemoveComponent(luabridge::LuaRef ref) {
	//Get the key
	if (!ref["key"].isString()) {
		//You know it's referencing either a particle or a rigidbody, but what.
		try {
			RigidBody* rigidbody = ref.cast<RigidBody*>();
			components_to_remove.push_back(bodyKey);
		}
		catch(luabridge::LuaException const& e) {
			components_to_remove.push_back(particleKey);
		}
	}
	else {
		std::string key = ref["key"];
		components.find(key)->second["enabled"] = false; //Problem is, this won't run till the end of the function call. I need this to run immediately. I have no idea what to do.
		components_to_remove.push_back(key);
	}
}



void ActorDB::setStart(std::string val) {
	//Add that string to the start thingies
	startComponents.insert(val);
}

void ActorDB::disableAll() {
	for (auto& [key, component] : components) {
		component["enabled"] = false;
	}
}