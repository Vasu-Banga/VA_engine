#include "NewScene.h"

//Reference static variables for linker
std::vector<std::string> NewScene::actorNames;
std::vector<std::vector<size_t, luabridge::LuaRef>> NewScene::actorComponents;
std::vector<bool> NewScene::persistant;
std::vector<std::pair<size_t, size_t>> NewScene::components_to_remove;
std::vector<std::pair<size_t, size_t>> NewScene::components_to_add;
std::vector<std::pair<size_t, size_t>> NewScene::starting_components;
std::unordered_map<std::string, size_t> NewScene::stringToComponentID;
std::vector<std::string> NewScene::componentIDToString;
std::vector<size_t> NewScene::actors_to_add;
std::vector<size_t> NewScene::actors_to_remove;
std::unordered_map<std::string, size_t> NewScene::stringToTemplateID;
std::vector<std::vector<size_t>> NewScene::templateComponents;
std::vector<std::string> NewScene::templateNames;
std::string NewScene::currentScene = "";


void NewScene::createActor(const rapidjson::Value& values) {
	
}

void NewScene::createActorFromTemplate(const rapidjson::Value& values, const std::string& templateName){
	
}

void NewScene::initializeScripts(){

}

void NewScene::loadScene(const std::string& sceneName){

}

void NewScene::start(){

}

void NewScene::update(){

}

void NewScene::lateUpdate(){

}

void NewScene::alterActors(){

}
