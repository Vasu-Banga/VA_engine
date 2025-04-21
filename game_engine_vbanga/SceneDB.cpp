//
//  SceneDB.cpp
//  game_engine
//
//  Created by Vasu Banga on 2/2/25.
//

#include "SceneDB.hpp"
#include <filesystem>
#include <iostream>
#include "include/rapidjson/filereadstream.h"
#include "ImageDB.h"
#include "Helper.h"
#include "TextDB.h"
#include <thread>
#include "Input.h"
#include "box2d/box2d.h"
#include "ParticleSystem.h"

/*
Gameplan: Change update to only iterate through characters that move
Characters can only move if they have some initial velocity, cause then they can just bounce and shit

Inside of update, we just iterate through all characters that have some trigger or contact value. Those are the only ones that matter

DONE - Have all actors accessible from an unordered_map. We can then just use vectors that have int values for the keys

DONE - Set up Images to all be loaded into a unordered_map, and then called. Will save so much storage, and then we can change all SDL_Texture* to that

*/



void SceneDB::EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table)
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

void ReadJsonFile(const std::string& path, rapidjson::Document& out_document)
{
	FILE* file_pointer = nullptr;
#ifdef _WIN32
	fopen_s(&file_pointer, path.c_str(), "rb");
#else
	file_pointer = fopen(path.c_str(), "rb");
#endif
	char buffer[65536];
	rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
	out_document.ParseStream(stream);
	std::fclose(file_pointer);

	if (out_document.HasParseError()) {
		//        rapidjson::ParseErrorCode errorCode = out_document.GetParseError();
		std::cout << "error parsing json at [" << path << "]" << std::endl;
		exit(0);
	}
}

void SceneDB::log(std::string value) {
	std::cout << value << std::endl;
}


void SceneDB::loadComponents() {
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Debug")
		.addFunction("Log", SceneDB::log)
		.endNamespace();
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<ActorDB>("Actor")
		.addFunction("GetName", &ActorDB::getName)
		.addFunction("GetID", &ActorDB::getKey)
		.addFunction("GetComponentByKey", &ActorDB::getComponentByKey)
		.addFunction("GetComponent", &ActorDB::getComponent)
		.addFunction("GetComponents", &ActorDB::getComponents)
		.addFunction("AddComponent", &ActorDB::AddComponent)
		.addFunction("RemoveComponent", &ActorDB::RemoveComponent)
		.endClass();
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Actor")
		.addFunction("Find", &SceneDB::Find)
		.addFunction("FindAll", &SceneDB::FindAll)
		.addFunction("Instantiate", &SceneDB::Instantiate)
		.addFunction("Destroy", &SceneDB::Destroy)
		.endNamespace();
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Application")
		.addFunction("Quit", &SceneDB::quit)
		.addFunction("Sleep", &SceneDB::sleep)
		.addFunction("GetFrame", Helper::GetFrameNumber)
		.addFunction("OpenURL", &SceneDB::openURL)
		.endNamespace();
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<glm::vec2>("vec2")
		.addProperty("x", &glm::vec2::x)
		.addProperty("y", &glm::vec2::y)
		.endClass();
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Input")
		.addFunction("GetKey", &Input::GetKey)
		.addFunction("GetKeyDown", &Input::GetKeyDown)
		.addFunction("GetKeyUp", &Input::GetKeyUp)
		.addFunction("GetMousePosition", &Input::GetMousePosition)
		.addFunction("GetMouseButton", &Input::GetMouseButton)
		.addFunction("GetMouseButtonDown", &Input::GetMouseDown)
		.addFunction("GetMouseButtonUp", &Input::GetMouseUp)
		.addFunction("GetMouseScrollDelta", &Input::GetMouseScrollDelta)
		.addFunction("HideCursor", &Input::HideCursor)
		.addFunction("ShowCursor", &Input::ShowCursor)
		.addFunction("GetControllerButton", &Input::GetControllerButton)
		.addFunction("GetControllerDown", &Input::GetControllerDown)
		.addFunction("GetControllerUp", &Input::GetControllerUp)
		.addFunction("GetControllerLeftStick", &Input::GetControllerLeftStick)
		.addFunction("GetControllerRightStick", &Input::GetControllerRightStick)
		.addFunction("GetControllerLeftTrigger", &Input::GetControllerLeftTrigger)
		.addFunction("GetControllerRightTrigger", &Input::GetControllerRightTrigger)
		.endNamespace();
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Text")
		.addFunction("Draw", &TextDB::DrawText)
		.endNamespace();
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Audio")
		.addFunction("Play", &AudioDB::PlayAudio)
		.addFunction("Halt", &AudioDB::Halt)
		.addFunction("SetVolume", &AudioDB::SetVolume)
		.endNamespace();
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Image")
		.addFunction("Draw", &ImageDB::Draw)
		.addFunction("DrawEx", &ImageDB::DrawEx)
		.addFunction("DrawUI", &ImageDB::DrawUI)
		.addFunction("DrawUIEx", &ImageDB::DrawUIEx)
		.addFunction("DrawPixel", &ImageDB::DrawPixel)
		.endNamespace();
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Camera")
		.addFunction("SetPosition", &ImageDB::setCamera)
		.addFunction("GetPositionX", &ImageDB::getPositionX)
		.addFunction("GetPositionY", &ImageDB::getPositionY)
		.addFunction("SetZoom", &ImageDB::setZoom)
		.addFunction("GetZoom", &ImageDB::getZoom)
		.endNamespace();
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Scene")
		.addFunction("Load", &SceneDB::Load)
		.addFunction("GetCurrent", &SceneDB::getCurrent)
		.addFunction("DontDestroy", &SceneDB::DontDestroy)
		.endNamespace();
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<b2Vec2>("Vector2")
		.addConstructor<void(*) (float, float)>()
		.addProperty("x", &b2Vec2::x)
		.addProperty("y", &b2Vec2::y)
		.addFunction("Normalize", &b2Vec2::Normalize)
		.addFunction("Length", &b2Vec2::Length)
		.addFunction("__add", &b2Vec2::operator_add)
		.addFunction("__sub", &b2Vec2::operator_sub)
		.addFunction("__mul", &b2Vec2::operator_mul)
		.endClass();
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Vector2")
		.addFunction("Distance", &b2Distance)
		.addFunction("Dot", static_cast<float (*)(const b2Vec2&, const b2Vec2&)>(&b2Dot))
		.endNamespace();
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<RigidBody>("Rigidbody")
		.addFunction("OnStart", &RigidBody::onStart)
		.addFunction("OnDestroy", &RigidBody::OnDestroy)
		.addFunction("LateUpdate", &RigidBody::lateUpdate)
		.addFunction("GetPosition", &RigidBody::GetPosition)
		.addFunction("GetRotation", &RigidBody::GetRotation)
		.addFunction("AddForce", &RigidBody::AddForce)
		.addFunction("SetVelocity", &RigidBody::SetVelocity)
		.addFunction("SetPosition", &RigidBody::SetPosition)
		.addFunction("SetRotation", &RigidBody::SetRotation)
		.addFunction("SetAngularVelocity", &RigidBody::SetAngularVelocity)
		.addFunction("SetGravityScale", &RigidBody::SetGravityScale)
		.addFunction("SetUpDirection", &RigidBody::SetUpDirection)
		.addFunction("SetRightDirection", &RigidBody::SetRightDirection)
		.addFunction("GetVelocity", &RigidBody::GetVelocity)
		.addFunction("GetAngularVelocity", &RigidBody::GetAngularVelocity)
		.addFunction("GetGravityScale", &RigidBody::GetGravityScale)
		.addFunction("GetUpDirection", &RigidBody::GetUpDirection)
		.addFunction("GetRightDirection", &RigidBody::GetRightDirection)
		.addProperty("x", &RigidBody::getX, &RigidBody::setX)
		.addProperty("y", &RigidBody::getY, &RigidBody::setY)
		.addProperty("rotation", &RigidBody::getRotation, &RigidBody::setRotation)
		.addProperty("gravity_scale", &RigidBody::getGravityScale, &RigidBody::setGravityScale)
		.endClass();
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<ParticleSystem>("ParticleSystem")
		.addFunction("OnUpdate", &ParticleSystem::onUpdate)
		.addFunction("OnStart", &ParticleSystem::onStart)
		.addFunction("Play", &ParticleSystem::Play)
		.addFunction("Stop", &ParticleSystem::Stop)
		.addFunction("Burst", &ParticleSystem::Burst)
		.addProperty("x", &ParticleSystem::getX, &ParticleSystem::setX)
		.addProperty("y", &ParticleSystem::getY, &ParticleSystem::setY)
		.addProperty("enabled", &ParticleSystem::getEnabled, &ParticleSystem::setEnabled)
		.addProperty("frames_between_bursts", &ParticleSystem::getFramesBetweenBursts, &ParticleSystem::setFramesBetweenBursts)
		.addProperty("burst_quantity", &ParticleSystem::getBurstQuantity, &ParticleSystem::setBurstQuantity)
		.addProperty("start_scale_min", &ParticleSystem::getStartScaleMin, &ParticleSystem::setStartScaleMin)
		.addProperty("start_scale_max", &ParticleSystem::getStartScaleMax, &ParticleSystem::setStartScaleMax)
		.addProperty("rotation_min", &ParticleSystem::getRotationMin, &ParticleSystem::setRotationMin)
		.addProperty("rotation_max", &ParticleSystem::getRotationMax, &ParticleSystem::setRotationMax)
		.addProperty("start_color_r", &ParticleSystem::getStartColorR, &ParticleSystem::setStartColorR)
		.addProperty("start_color_g", &ParticleSystem::getStartColorG, &ParticleSystem::setStartColorG)
		.addProperty("start_color_b", &ParticleSystem::getStartColorB, &ParticleSystem::setStartColorB)
		.addProperty("start_color_a", &ParticleSystem::getStartColorA, &ParticleSystem::setStartColorA)
		.addProperty("sorting_order", &ParticleSystem::getSortingOrder, &ParticleSystem::setSortingOrder)
		.addProperty("emit_radius_min", &ParticleSystem::getEmitRadiusMin, &ParticleSystem::setEmitRadiusMin)
		.addProperty("emit_radius_max", &ParticleSystem::getEmitRadiusMax, &ParticleSystem::setEmitRadiusMax)
		.addProperty("emit_angle_min", &ParticleSystem::getEmitAngleMin, &ParticleSystem::setEmitAngleMin)
		.addProperty("emit_angle_max", &ParticleSystem::getEmitAngleMax, &ParticleSystem::setEmitAngleMax)
		.addProperty("image", &ParticleSystem::getParticleName, &ParticleSystem::setParticleName)
		.addProperty("duration_frames", &ParticleSystem::getDurationFrames, &ParticleSystem::setDurationFrames)
		.addProperty("start_speed_min", &ParticleSystem::getStartSpeedMin, &ParticleSystem::setStartSpeedMin)
		.addProperty("start_speed_max", &ParticleSystem::getStartSpeedMax, &ParticleSystem::setStartSpeedMax)
		.addProperty("rotation_speed_min", &ParticleSystem::getRotationSpeedMin, &ParticleSystem::setRotationSpeedMin)
		.addProperty("rotation_speed_max", &ParticleSystem::getRotationSpeedMax, &ParticleSystem::setRotationSpeedMax)
		.addProperty("gravity_scale_x", &ParticleSystem::getGravityScaleX, &ParticleSystem::setGravityScaleX)
		.addProperty("gravity_scale_y", &ParticleSystem::getGravityScaleY, &ParticleSystem::setGravityScaleY)
		.addProperty("drag_factor", &ParticleSystem::getDragFactor, &ParticleSystem::setDragFactor)
		.addProperty("angular_drag_factor", &ParticleSystem::getAngularDragFactor, &ParticleSystem::setAngularDragFactor)
		.addProperty("end_scale", &ParticleSystem::getEndScale, &ParticleSystem::setEndScale)
		.addProperty("end_color_r", &ParticleSystem::getEndColorR, &ParticleSystem::setEndColorR)
		.addProperty("end_color_g", &ParticleSystem::getEndColorG, &ParticleSystem::setEndColorG)
		.addProperty("end_color_b", &ParticleSystem::getEndColorB, &ParticleSystem::setEndColorB)
		.addProperty("end_color_a", &ParticleSystem::getEndColorA, &ParticleSystem::setEndColorA)
		.endClass();
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Physics")
		.addFunction("Raycast", &RigidBody::Raycast)
		.addFunction("RaycastAll", &RigidBody::RaycastAll)
		.endNamespace();
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Event")
		.addFunction("Publish", &SceneDB::EventPublish)
		.addFunction("Subscribe", &SceneDB::EventSubscribe)
		.addFunction("Unsubscribe", &SceneDB::EventUnsubscribe)
		.endNamespace();
	const std::string componentDir = "resources/component_types";
	if (!std::filesystem::exists(componentDir)) return;
	for (const auto& entry : std::filesystem::directory_iterator(componentDir)) {
		if (entry.path().extension() == ".lua") {
			std::string filepath = entry.path().string();
			std::string fileName = entry.path().stem().string();


			if (luaL_dofile(lua_state, filepath.c_str()) != LUA_OK) {
				std::cout << "problem with lua file " << fileName;
				exit(0);
			}

			loadedComponents.insert(fileName);

			}
		}
	}

void SceneDB::quit() {
	exit(0);
}
void SceneDB::sleep(int milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
void SceneDB::openURL(std::string url) {
#if defined(_WIN32) || defined(_WIN64)
	std::system(("start " + url).c_str());
#elif defined(__APPLE__) || defined(__MACH__)
	std::system(("open " + url).c_str());
#elif defined(__linux__)
	std::system(("xdg-open " + url).c_str());
#endif
}


void SceneDB::loadValues(const rapidjson::Value& values, ActorDB* tempActor, bool isTemplate) {
	//Helper function to process JSON files, so I don't have to write the same fucking code in both template and scene
	if (values.HasMember("name")) {
		tempActor->setName(values["name"].GetString());
	}
	if (values.HasMember("components")) {
		//YAYYYYYYYY I GET TO PROCESS COMPONENTS (i hate this)

		//The issue i am facing currently- actor is loaded in already through template, but when I call this, I rewrite it. Need to fix this
		for (rapidjson::SizeType i = 0; i < values["components"].MemberCount(); i++) {
			//key is what?
			const rapidjson::Value::ConstMemberIterator& itr = values["components"].MemberBegin() + i;
			std::string componentName = itr->name.GetString();
			const rapidjson::Value& componentValue = itr->value;
			std::optional<luabridge::LuaRef*> existingActor = tempActor->componentExists(componentName);
			// Create a new empty instance table
			if (!existingActor.has_value()) {
				//New component not previously loaded!
				std::string value = componentValue["type"].GetString();

				//Now check if exists, if not, load it in
				if (value != "Rigidbody" && value != "ParticleSystem" && loadedComponents.count(value) == 0) {
					//Load in the component
					std::cout << "error: failed to locate component " << value;
					exit(0);
				}
				luabridge::LuaRef componentInstance = luabridge::newTable(lua_state);
				//componentInstance = new luabridge::LuaRef(luabridge::newTable(lua_state));
				if (value == "Rigidbody") {
					//Put code to initialize a Rigidbody value in here
					RigidBody* newVal = new RigidBody();
					tempActor->setRigidBody(newVal, componentName);
					componentInstance = luabridge::LuaRef(lua_state, newVal);
					//Next component

					newVal->setActor(tempActor);

					for (rapidjson::Value::ConstMemberIterator itr2 = componentValue.MemberBegin(); itr2 != componentValue.MemberEnd(); ++itr2) {
						//Something goes in here? Iterate through all values that this thingy has
						//All overrides
						std::string key = itr2->name.GetString();
						if (key == "x") {
							newVal->setX(itr2->value.GetFloat());
						}
						else
							if (key == "y") {
								newVal->setY(itr2->value.GetFloat());
							}
							else
								if (key == "body_type") {
									newVal->setBodyType(itr2->value.GetString());
								}
								else
									if (key == "precise") {
										newVal->setPrecise(itr2->value.GetBool());
									}
									else
										if (key == "gravity_scale") {
											newVal->setGravityScale(itr2->value.GetFloat());
										}
										else
											if (key == "density") {
												newVal->setDensity(itr2->value.GetFloat());
											}
											else
												if (key == "angular_friction") {
													newVal->setAngularFriction(itr2->value.GetFloat());
												}
												else
													if (key == "rotation") {
														newVal->setRotation(itr2->value.GetFloat());
													}
													else
														if (key == "has_collider") {
															//tempActor->setCollider(itr2->value.GetBool());
															newVal->setCollider(itr2->value.GetBool());
														}
														else
															if (key == "has_trigger") {
																//tempActor->setTrigger(itr2->value.GetBool());
																newVal->setTrigger(itr2->value.GetBool());

															}
															else
																if (key == "width") {
																	newVal->setWidth(itr2->value.GetFloat());
																}
																else
																	if (key == "height") {
																		newVal->setHeight(itr2->value.GetFloat());
																	}
																	else
																		if (key == "collider_type") {
																			newVal->setColliderType(itr2->value.GetString());
																		}
																		else
																			if (key == "radius") {
																				newVal->setRadius(itr2->value.GetFloat());
																			}
																			else
																				if (key == "friction") {
																					newVal->setFriction(itr2->value.GetFloat());
																				}
																				else
																					if (key == "bounciness") {
																						newVal->setBounciness(itr2->value.GetFloat());
																					}
						if (key == "trigger_radius") {
							newVal->setTriggerRadius(itr2->value.GetFloat());
						}
						if (key == "trigger_width") {
							newVal->setTriggerWidth(itr2->value.GetFloat());
						}
						if (key == "trigger_height") {
							newVal->setTriggerHeight(itr2->value.GetFloat());
						}
						if (key == "trigger_type") {
							newVal->setTriggerType(itr2->value.GetString());
						}
						//Only handling these objects for now. If I fail any test case, I'll change it later, but for now I think this is good

					}

					//TODO this please come back to this don't forget yippee
				}
				else if (value == "ParticleSystem") {
					ParticleSystem* newVal = new ParticleSystem();
					tempActor->setParticleSystem(newVal, componentName);
					componentInstance = luabridge::LuaRef(lua_state, newVal);



					//Load up particle system
					for (rapidjson::Value::ConstMemberIterator itr2 = componentValue.MemberBegin(); itr2 != componentValue.MemberEnd(); ++itr2) {
						std::string key = itr2->name.GetString();

						// Check the keys and set the corresponding properties
						if (key == "x") {
							newVal->setX(itr2->value.GetFloat());
						}
						else if (key == "y") {
							newVal->setY(itr2->value.GetFloat());
						}
						else if (key == "frames_between_bursts") {
							newVal->setFramesBetweenBursts(itr2->value.GetInt());
						}
						else if (key == "burst_quantity") {
							newVal->setBurstQuantity(itr2->value.GetInt());
						}
						else if (key == "start_scale_min") {
							newVal->setStartScaleMin(itr2->value.GetFloat());
						}
						else if (key == "start_scale_max") {
							newVal->setStartScaleMax(itr2->value.GetFloat());
						}
						else if (key == "rotation_min") {
							newVal->setRotationMin(itr2->value.GetFloat());
						}
						else if (key == "rotation_max") {
							newVal->setRotationMax(itr2->value.GetFloat());
						}
						else if (key == "start_color_r") {
							newVal->setStartColorR(itr2->value.GetInt());
						}
						else if (key == "start_color_g") {
							newVal->setStartColorG(itr2->value.GetInt());
						}
						else if (key == "start_color_b") {
							newVal->setStartColorB(itr2->value.GetInt());
						}
						else if (key == "start_color_a") {
							newVal->setStartColorA(itr2->value.GetInt());
						}
						else if (key == "sorting_order") {
							newVal->setSortingOrder(itr2->value.GetInt());
						}
						else if (key == "emit_radius_min") {
							newVal->setEmitRadiusMin(itr2->value.GetFloat());
						}
						else if (key == "emit_radius_max") {
							newVal->setEmitRadiusMax(itr2->value.GetFloat());
						}
						else if (key == "emit_angle_min") {
							newVal->setEmitAngleMin(itr2->value.GetFloat());
						}
						else if (key == "emit_angle_max") {
							newVal->setEmitAngleMax(itr2->value.GetFloat());
						}
						else if (key == "image") {
							newVal->setParticleName(itr2->value.GetString());
						}
						else if (key == "duration_frames") {
							newVal->setDurationFrames(itr2->value.GetInt());
						}
						else if (key == "start_speed_min") {
							newVal->setStartSpeedMin(itr2->value.GetFloat());
						}
						else if (key == "start_speed_max") {
							newVal->setStartSpeedMax(itr2->value.GetFloat());
						}
						else if (key == "rotation_speed_min") {
							newVal->setRotationSpeedMin(itr2->value.GetFloat());
						}
						else if (key == "rotation_speed_max") {
							newVal->setRotationSpeedMax(itr2->value.GetFloat());
						}
						else if (key == "gravity_scale_x") {
							newVal->setGravityScaleX(itr2->value.GetFloat());
						}
						else if (key == "gravity_scale_y") {
							newVal->setGravityScaleY(itr2->value.GetFloat());
						}
						else if (key == "drag_factor") {
							newVal->setDragFactor(itr2->value.GetFloat());
						}
						else if (key == "angular_drag_factor") {
							newVal->setAngularDragFactor(itr2->value.GetFloat());
						}
						else if (key == "end_scale") {
							newVal->setEndScale(itr2->value.GetFloat());
						}
						else if (key == "end_color_r") {
							newVal->setEndColorR(itr2->value.GetInt());
						}
						else if (key == "end_color_g") {
							newVal->setEndColorG(itr2->value.GetInt());
						}
						else if (key == "end_color_b") {
							newVal->setEndColorB(itr2->value.GetInt());
						}
						else if (key == "end_color_a") {
							newVal->setEndColorA(itr2->value.GetInt());
						}
					}
				}
				else {
					luabridge::LuaRef componentTemplate = luabridge::getGlobal(lua_state, value.c_str());
					EstablishInheritance(componentInstance, componentTemplate);
					componentInstance["key"] = componentName;
					componentInstance["enabled"] = true; //This ensures every component has it's own enabled value
					componentInstance["actor"] = tempActor;
					//loop through all values, insert into component
					for (rapidjson::Value::ConstMemberIterator itr2 = componentValue.MemberBegin(); itr2 != componentValue.MemberEnd(); ++itr2) {
						//Something goes in here? Iterate through all values that this thingy has
						//All overrides
						std::string key = itr2->name.GetString();
						if (itr2->value.IsString()) {
							componentInstance[key] = itr2->value.GetString();
						}
						else if (itr2->value.IsFloat()) {
							componentInstance[key] = itr2->value.GetFloat();
						}
						else if (itr2->value.IsInt()) {
							componentInstance[key] = itr2->value.GetInt();
						}
						else if (itr2->value.IsBool()) {
							componentInstance[key] = itr2->value.GetBool();
						}

						//Only handling these objects for now. If I fail any test case, I'll change it later, but for now I think this is good

					}
				}
				tempActor->addComponent(componentName, componentInstance);
			}
			else {
				//Now I just override values, and that's it.
				luabridge::LuaRef& prevVal = *existingActor.value();
				prevVal["key"] = componentName;
				for (rapidjson::Value::ConstMemberIterator itr2 = componentValue.MemberBegin(); itr2 != componentValue.MemberEnd(); ++itr2) {
					//Something goes in here? Iterate through all values that this thingy has
					std::string key = itr2->name.GetString();
					if (itr2->value.IsString()) {
						prevVal[key] = itr2->value.GetString();
					}
					else if (itr2->value.IsFloat()) {
						prevVal[key] = itr2->value.GetFloat();
					}
					else if (itr2->value.IsInt()) {
						prevVal[key] = itr2->value.GetInt();
					}
					else if (itr2->value.IsBool()) {
						prevVal[key] = itr2->value.GetBool();
					}


					//Only handling these objects for now. If I fail any test case, I'll change it later, but for now I think this is good
				}

			}
		}
	}

	/*for (rapidjson::SizeType i = 0; i < values["components"].MemberCount(); i++) {
		const rapidjson::Value::ConstMemberIterator& itr = values["components"].MemberBegin() + i;
		std::string componentName = itr->name.GetString();
		std::optional<luabridge::LuaRef*> existingActor = tempActor->componentExists(componentName);
		if (existingActor.has_value()) {
			luabridge::LuaRef& prevVal = *existingActor.value();
			prevVal["actor"] = tempActor;
		}
	}*/
}

SDL_Renderer* SceneDB::renderer;

void SceneDB::loadTemplate(const std::string& templateName, ActorDB* tempActor) {
	if (!std::filesystem::exists("resources/actor_templates/" + templateName + ".template")) {
		std::cout << "error: template " << templateName << " is missing";
		exit(0);
	}
	rapidjson::Document templateVals;
	ReadJsonFile("resources/actor_templates/" + templateName + ".template", templateVals);

	loadValues(templateVals, tempActor, true);

	if (templates.count(templateName) == 0) {
		templates.insert({ templateName,tempActor });
	}
}


void SceneDB::loadScene(std::string sceneName, const bool& initial = false) {
	if (!std::filesystem::exists("resources/scenes/" + sceneName + ".scene")) {
		std::cout << "error: scene " << sceneName << " is missing";
		exit(0);
	}
	this->sceneName = sceneName;
	if (!initial) {
		for (auto& [key, actor] : sceneActors) {
			if (!actor->getPersistence()) {
				actor->Delete();
				delete actor;
				sceneActors.erase(key);
			}
		}
		currentScene.RemoveAllMembers();
	}
	this->renderer = renderer;
	ReadJsonFile("resources/scenes/" + sceneName + ".scene", currentScene);
	//parse through all actors, put them in there
	numActors = currentScene["actors"].Size();
	for (rapidjson::SizeType i = 0; i < currentScene["actors"].Size(); i++) {
		ActorDB* tempActor;

		//Template code taken out, put back in if necessary

		if (currentScene["actors"][i].HasMember("template")) {
			//Check if we already have this template loaded in
			std::string templateName = currentScene["actors"][i]["template"].GetString();
			//if (templates.count(templateName) == 0) {
			//	tempActor = new ActorDB(i);
			//	//Load in a template
			//	loadTemplate(templateName, tempActor);
			//}
			//else {
			//	tempActor = new ActorDB(*templates[templateName]);
			//}
			tempActor = new ActorDB(i);
			loadTemplate(templateName, tempActor);
		}
		else {
			tempActor = new ActorDB(i);
		}
		std::string currName;
		//Now, if an actor comes out with template components, we need to reinitialize it before overriding it, so we don't mess up our templates
		tempActor->updateTemplates(); //I am going to lose my goddamn mind
		loadValues(currentScene["actors"][i], tempActor, false);
		tempActor->setKey(i);
		sceneActors.insert({ i,tempActor });
	}
}

void SceneDB::start() {
	nextScene = sceneName;
	for (auto& [key, actor] : sceneActors) {
		actor->start();
	}
}

void SceneDB::update() {
	//Well, as it turns out, none of it matters in homework 7. Go me!
	for (auto& [key, actor] : sceneActors) {
		actor->update();
	}


}

void SceneDB::lateUpdate() {
	for (auto& [key, actor] : sceneActors) {
		actor->lateUpdate();
	}
	for (auto& [type, comp, func] : toSubscribe) {
		SceneDB::eventSubscriptions[type].emplace_back(comp, func);
	}
	toSubscribe.clear();

	// Process unsubscriptions
	for (auto& tuple : toUnsubscribe) {
		const std::string& type = std::get<0>(tuple);
		const luabridge::LuaRef& comp = std::get<1>(tuple);
		const luabridge::LuaRef& func = std::get<2>(tuple);

		auto& vec = SceneDB::eventSubscriptions[type];
		vec.erase(std::remove_if(vec.begin(), vec.end(),
			[&](const std::pair<luabridge::LuaRef, luabridge::LuaRef>& pair) {
				return pair.first == comp && pair.second == func;
			}), vec.end());
	}
	toUnsubscribe.clear();
}


SceneDB* SceneDB::currentInstance;

SceneDB::SceneDB(const int& tempWidth, const int& tempHeight) {
	width = tempWidth;
	height = tempHeight;
	currentInstance = this;
}

SceneDB::~SceneDB() {
	if (currentInstance == this) {
		currentInstance = nullptr;
	}
}

lua_State* SceneDB::lua_state;

void SceneDB::setLuaState(lua_State* val) {
	lua_state = val;
}

luabridge::LuaRef SceneDB::Find(std::string name) {
	for (auto& [key, actor] : currentInstance->sceneActors) {
		if (actor->getName() == name && !(actor->getDelete())) {
			return luabridge::LuaRef(lua_state, &actor);
		}
	}
	for (ActorDB* actor : currentInstance->actors_to_add) {
		if (actor->getName() == name && !(actor->getDelete())) {
			return luabridge::LuaRef(lua_state, &actor);
		}
	}
	return luabridge::LuaRef(lua_state); // nil if not found
}

luabridge::LuaRef SceneDB::FindAll(std::string name) {
	luabridge::LuaRef results = luabridge::newTable(lua_state);
	int index = 1;
	for (auto& [key, actor] : currentInstance->sceneActors) {
		if (actor->getName() == name && !(actor->getDelete())) {
			results[index++] = &actor;
		}
	}
	for (ActorDB* actor : currentInstance->actors_to_add) {
		if (actor->getName() == name && !(actor->getDelete())) {
			results[index++] = &actor;
		}
	}
	return results; // empty table if none found
}

luabridge::LuaRef SceneDB::Instantiate(std::string templateName) {
	ActorDB* newActor;
	/*if (currentInstance->templates.count(templateName) == 0) {
		newActor = new ActorDB(currentInstance->numActors);
		currentInstance->loadTemplate(templateName,newActor);
	}
	else {
		newActor = new ActorDB(*(currentInstance->templates[templateName]));
	}*/
	newActor = new ActorDB(currentInstance->numActors);
	currentInstance->loadTemplate(templateName, newActor);
	newActor->setKey(currentInstance->numActors++);
	newActor->updateTemplates();
	newActor->setRun(false);
	currentInstance->actors_to_add.push_back(newActor);
	//currentInstance->sceneActors.push_back(newActor); //Caused a massive memory leak

	return luabridge::LuaRef(lua_state, newActor);
}

void SceneDB::DontDestroy(luabridge::LuaRef reference) {
	ActorDB* actor = reference.cast<ActorDB*>();
	if (!actor) {
		std::cout << "error: Destroy could not cast to ActorDB*";
		return;
	}
	actor->setPersistence(true);
}

void SceneDB::Destroy(luabridge::LuaRef reference) {
	ActorDB* actor = reference.cast<ActorDB*>();
	if (!actor) {
		std::cout << "error: Destroy could not cast to ActorDB*";
		return;
	}
	int key = actor->getKey();
	//Now to remove this value

	//This is not how you find the actor. If you have a bunch of actors, and then delete one, the key is no longer the index.
	//Find another way to do this.
	//I guess I gotta use another fucking unordered_map
	actor->setDelete(true);
	actor->disableAll();
	currentInstance->actors_to_remove.push_back(key);
}

void SceneDB::alterActors() {
	//Add all values that need to be added
	for (ActorDB* actor : actors_to_add) {
		sceneActors.insert({ actor->getKey(), actor });
	}
	actors_to_add.clear();
	for (int actor : actors_to_remove) {
		sceneActors[actor]->Delete(); //okay there we go yippeee
		delete sceneActors[actor];
		sceneActors.erase(actor);
	}
	actors_to_remove.clear();
	for (auto& [key, component] : sceneActors) {
		component->alterContainer();
	}
}


void SceneDB::checkForChange() {
	//Check if the scene changes!
	if (nextScene == sceneName) return;
	loadScene(nextScene, false);
}


std::unordered_map<std::string, std::vector<std::pair<luabridge::LuaRef, luabridge::LuaRef>>> SceneDB::eventSubscriptions;
std::vector<std::tuple<std::string, luabridge::LuaRef, luabridge::LuaRef>> SceneDB::toSubscribe;
std::vector<std::tuple<std::string, luabridge::LuaRef, luabridge::LuaRef>> SceneDB::toUnsubscribe;

void SceneDB::EventPublish(std::string type, luabridge::LuaRef eventObject) {
	for (auto& [component, func] : eventSubscriptions[type]) {
		if (component.isTable() && func.isFunction()) {
			try {
				func(component, eventObject);
			}
			catch (luabridge::LuaException const& e) {
				std::cout << "Lua error during Event.Publish: " << e.what();
			}
		}
	}
}

void SceneDB::EventSubscribe(std::string type, luabridge::LuaRef component, luabridge::LuaRef func) {
	toSubscribe.emplace_back(type, component, func);
}

void SceneDB::EventUnsubscribe(std::string type, luabridge::LuaRef component, luabridge::LuaRef func) {
	toUnsubscribe.emplace_back(type, component, func);
}