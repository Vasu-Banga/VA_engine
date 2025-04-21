#ifndef RIGIDBODY_H
#define RIGIDBODY_H


#include "box2d/box2d.h"
#include <string>
#include <iostream>
#include "glm/glm.hpp"
class ActorDB;
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

class CollisionDetector : public b2ContactListener {
public:
	void BeginContact(b2Contact* contact) override;
	void EndContact(b2Contact* contact) override;
};

class RigidBody {
private:
	static b2World* world;
	float x = 0.0f;
	float y = 0.0f;
	float width = 1.0f;
	float height = 1.0f;
	std::string body_type = "dynamic";
	bool precise = true;
	float gravity_scale = 1.0f;
	float density = 1.0f;
	float angular_friction = 0.3f;
	float rotation = 0.0f;

	bool has_collider = true;
	bool has_trigger = true;

	std::string collider_type = "box";
	float radius = 0.5f;
	float friction = 0.3f;
	float bounciness = 0.3f;

	float trigger_width = 1.0f;
	float trigger_height = 1.0f;
	float trigger_radius = 0.5f;
	std::string trigger_type = "box";

	b2Body* body;
	ActorDB* actor = nullptr;
	static CollisionDetector* contactListener;
public:
	RigidBody();
	static void step();
	void setActor(ActorDB* val) { actor = val; }
	void setX(float val) { x = val; }
	void setY(float val) { y = val; }
	float getX() const { return x; }
	float getY() const { return y; }
	float getRotation() const { return rotation; }
	float getGravityScale() const { return gravity_scale; }
	void setBodyType(std::string val) { body_type = val; }
	void setPrecise(bool val) { precise = val; }
	void setGravityScale(float val) { gravity_scale = val; }
	void setDensity(float val) { density = val; }
	void setAngularFriction(float val) { angular_friction = val; }
	void setRotation(float val) { rotation = val; }
	void setCollider(bool val) { has_collider = val; }
	void setTrigger(bool val) { has_trigger = val; }
	void setWidth(float val) { width = val; }
	void setHeight(float val) { height = val; }
	void setColliderType(std::string val) { collider_type = val; }
	void setRadius(float val) { radius = val; }
	void setFriction(float val) { friction = val; }
	void setBounciness(float val) { bounciness = val; }
	void setTriggerWidth(float val) { trigger_width = val; }
	void setTriggerHeight(float val) { trigger_height = val; }
	void setTriggerRadius(float val) { trigger_radius = val; }
	void setTriggerType(std::string val) { trigger_type = val; }
	//Set up a LateUpdate() function so that it can actually call step then
	//Set up an OnStart() function that will create world if it doesn't already exist
	void onStart();
	void lateUpdate();
	b2Vec2 GetPosition() { if (!body) { return b2Vec2(x, y); } return body->GetPosition(); }
	float GetRotation() { if (!body) { return rotation; } return (body->GetAngle() * (180.0f / b2_pi)); }
	void AddForce(b2Vec2 value) { body->ApplyForceToCenter(value, true); }
	void SetVelocity(b2Vec2 value) { body->SetLinearVelocity(value); }
	void SetPosition(b2Vec2 value) { if (!body) { x = value.x; y = value.y; return; } body->SetTransform(value, rotation); }
	void SetRotation(float value) { if (!body) { rotation = value; return; }body->SetTransform(GetPosition(), value * (b2_pi / 180.0f)); }
	void SetAngularVelocity(float value) { body->SetAngularVelocity(value * (b2_pi / 180.0f)); }
	void SetGravityScale(float value) {
		if (!body) { gravity_scale = value; return; } body->SetGravityScale(value);
	}
	void SetUpDirection(b2Vec2 value) {
		value.Normalize();
		float angle = glm::atan(value.y, -value.x);
		rotation = -angle - (b2_pi / 2.0f);
		body->SetTransform(GetPosition(), rotation);
	}
	void SetRightDirection(b2Vec2 value) {
		value.Normalize();
		float angle = glm::atan(value.y, -value.x);
		rotation = -angle + (b2_pi);
		body->SetTransform(GetPosition(), rotation);
	}
	b2Vec2 GetVelocity() { return body->GetLinearVelocity(); }
	float GetAngularVelocity() { return body->GetAngularVelocity() * (180.0f / b2_pi); }
	float GetGravityScale() {
		if (!body) { return gravity_scale; } return body->GetGravityScale();
	}
	b2Vec2 GetUpDirection() {
		float angle = body->GetAngle() - (b2_pi / 2.0f);
		b2Vec2 result = b2Vec2(glm::cos(angle), glm::sin(angle));
		result.Normalize();
		return result;
	}
	b2Vec2 GetRightDirection() {
		float angle = body->GetAngle();
		b2Vec2 result = b2Vec2(glm::cos(angle), glm::sin(angle));
		result.Normalize();
		return result;
	}

	void OnDestroy();
	static luabridge::LuaRef Raycast(b2Vec2 pos, b2Vec2 dir, float dist);
	static luabridge::LuaRef RaycastAll(b2Vec2 pos, b2Vec2 dir, float dist);
};

#endif