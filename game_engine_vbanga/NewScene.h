#pragma once
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "include/rapidjson/document.h"


/*

///////////////////////////////////////////////////////////////////////////////////////////////////////////

What do I hope to accomplish with this:

Migration towards DoD for the entire engine

Incredibly enhanced runtime speeds

Removal of redundant code (DRY)

No function called by an actual game to take any less than O(log n) to run

The runtime of any frame loop should ideally run in O(nlogn) time, reduced by L1 and L2 cache ideally

///////////////////////////////////////////////////////////////////////////////////////////////////////////

*/



class NewScene
{
public:
	struct Actor {
		size_t key;
	};


	//SCENE FUNCTIONS

	
	//Actor generation
	static void createActor(const rapidjson::Value& values);
	static void createActorFromTemplate(const rapidjson::Value& values, const std::string& templateName);

	//Scene generation
	static void initializeScripts();
	static void loadScene(const std::string& sceneName);



	//ACTOR FUNCTIONS


	//Lifecycle functions
	static void start();
	static void update();
	static void lateUpdate();
	static void alterActors();

private:
	//Static variables
	static float zoomFactor;
	static int componentCount;
	static lua_State* lua_state;


	//ACTORDB GOES BELOW


	//All primary indicies refer to that specific actor
	static std::vector<std::string> actorNames;
	static std::vector<std::vector<size_t, luabridge::LuaRef>> actorComponents; //Second index refers to key, which is calculated through stringToComponentID*
	static std::vector<bool> persistant;

	//Actor Alter Values
	static std::vector<std::pair<size_t, size_t>> components_to_remove;
	static std::vector<std::pair<size_t, size_t>> components_to_add;
	static std::vector<std::pair<size_t, size_t>> starting_components;

	//Actor Helper Values
	static std::unordered_map<std::string, size_t> stringToComponentID; //Component strings get converted to size_ts. More cache effective
	static std::vector<std::string> componentIDToString;


	//SCENEDB GOES BELOW

	//Scene Alter Values
	static std::vector<size_t> actors_to_add; //Index refers to template key
	static std::vector<size_t> actors_to_remove; //Index refers to actor key

	//Scene Helper Values
	static std::unordered_map<std::string, size_t> stringToTemplateID;
	static std::vector<std::vector<size_t>> templateComponents;
	static std::vector<std::string> templateNames;

	//Scene Values
	static std::string currentScene;
};

/*

For reference, you decided to include the size_t in the pair for the components so that you could still be able to sort, an integral necessity.



*/