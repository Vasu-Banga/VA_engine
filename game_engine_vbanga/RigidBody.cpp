#include "RigidBody.h"
#include "box2d/include/box2d/box2d.h"
#include "ActorDB.h"

b2World* RigidBody::world = nullptr;
CollisionDetector* RigidBody::contactListener;



void CollisionDetector::BeginContact(b2Contact* contact) {
	auto* fixA = contact->GetFixtureA();
	auto* fixB = contact->GetFixtureB();
	ActorDB* actorA = reinterpret_cast<ActorDB*>(fixA->GetUserData().pointer);
	ActorDB* actorB = reinterpret_cast<ActorDB*>(fixB->GetUserData().pointer);
	if (!actorA || !actorB) return;

	b2WorldManifold manifold;
	contact->GetWorldManifold(&manifold);
	b2Vec2 point = manifold.points[0];
	b2Vec2 normal = manifold.normal;
	b2Vec2 sentinel(-999.0f, -999.0f);
	b2Vec2 relVelAB = fixA->GetBody()->GetLinearVelocity() - fixB->GetBody()->GetLinearVelocity();
	b2Vec2 relVelBA = relVelAB;

	lua_State* L = ActorDB::getLuaState();

	luabridge::LuaRef colA = luabridge::newTable(L);
	colA["other"] = actorB;
	colA["relative_velocity"] = relVelAB;

	luabridge::LuaRef colB = luabridge::newTable(L);
	colB["other"] = actorA;
	colB["relative_velocity"] = relVelBA;

	//Checking for everything
	/*

	If both aren't sensors, then the component has collided

	Otherwise, if there's some sensor

	*/
	//std::cout << "Collision between " << actorA->getName() << " and " << actorB->getName() << " is between a sensor (a) " << fixA->IsSensor() << " and sensor (b) " << fixB->IsSensor() << std::endl;
	if (!fixA->IsSensor() && !fixB->IsSensor()) {
		//std::cout << "Point is at " << point.x << ", " << point.y << std::endl;
		colA["point"] = point;
		colA["normal"] = normal;
		colB["point"] = point;
		colB["normal"] = normal;
		for (const auto& [key, comp] : actorA->getComponentsMap()) {
			if (comp["OnCollisionEnter"].isFunction()) {
				try { comp["OnCollisionEnter"](comp, colA); }
				catch (luabridge::LuaException const& e) {
					std::cout << "Lua error in OnCollisionEnter (A): " << e.what();
				}
			}
		}

		for (const auto& [key, comp] : actorB->getComponentsMap()) {
			if (comp["OnCollisionEnter"].isFunction()) {
				try { comp["OnCollisionEnter"](comp, colB); }
				catch (luabridge::LuaException const& e) {
					std::cout << "Lua error in OnCollisionEnter (B): " << e.what();
				}
			}
		}
	}

	if (fixA->IsSensor() && fixB->IsSensor()) {
		colA["point"] = sentinel;
		colA["normal"] = sentinel;
		colB["point"] = sentinel;
		colB["normal"] = sentinel;

		for (const auto& [key, comp] : actorA->getComponentsMap()) {
			if (comp["OnTriggerEnter"].isFunction()) {
				try { comp["OnTriggerEnter"](comp, colA); }
				catch (luabridge::LuaException const& e) {
					std::cout << "Lua error in OnTriggerEnter (A): " << e.what();
				}
			}
		}

		for (const auto& [key, comp] : actorB->getComponentsMap()) {
			if (comp["OnTriggerEnter"].isFunction()) {
				try { comp["OnTriggerEnter"](comp, colB); }
				catch (luabridge::LuaException const& e) {
					std::cout << "Lua error in OnTriggerEnter (B): " << e.what();
				}
			}
		}
	}


}

void CollisionDetector::EndContact(b2Contact* contact) {
	auto* fixA = contact->GetFixtureA();
	auto* fixB = contact->GetFixtureB();

	auto* actorA = reinterpret_cast<ActorDB*>(fixA->GetUserData().pointer);
	auto* actorB = reinterpret_cast<ActorDB*>(fixB->GetUserData().pointer);
	if (!actorA || !actorB) return;
	b2Vec2 relVelAB = fixA->GetBody()->GetLinearVelocity() - fixB->GetBody()->GetLinearVelocity();
	b2Vec2 relVelBA = relVelAB;

	lua_State* L = ActorDB::getLuaState();
	b2Vec2 sentinel(-999.0f, -999.0f);

	luabridge::LuaRef colA = luabridge::newTable(L);
	colA["other"] = actorB;
	colA["point"] = sentinel;
	colA["normal"] = sentinel;
	colA["relative_velocity"] = relVelAB;

	luabridge::LuaRef colB = luabridge::newTable(L);
	colB["other"] = actorA;
	colB["point"] = sentinel;
	colB["normal"] = sentinel;
	colB["relative_velocity"] = relVelBA;

	//Check for collision between the two bodies that collided
	if (!fixA->IsSensor() && !fixB->IsSensor()) {
		for (const auto& [key, comp] : actorA->getComponentsMap()) {
			if (comp["OnCollisionExit"].isFunction()) {
				try { comp["OnCollisionExit"](comp, colA); }
				catch (luabridge::LuaException const& e) {
					std::cout << "Lua error in OnCollisionExit (A): " << e.what() << "\n";
				}
			}
		}

		for (const auto& [key, comp] : actorB->getComponentsMap()) {
			if (comp["OnCollisionExit"].isFunction()) {
				try { comp["OnCollisionExit"](comp, colB); }
				catch (luabridge::LuaException const& e) {
					std::cout << "Lua error in OnCollisionExit (B): " << e.what() << "\n";
				}
			}
		}
	}

	//If one of the bodies is a trigger, that's triggered too. Both don't need to be a sensor to be triggered
	if (fixA->IsSensor() && fixB->IsSensor()) {
		for (const auto& [key, comp] : actorA->getComponentsMap()) {
			if (comp["OnTriggerExit"].isFunction()) {
				try { comp["OnTriggerExit"](comp, colA); }
				catch (luabridge::LuaException const& e) {
					std::cout << "Lua error in OnTriggerExit (A): " << e.what();
				}
			}
		}

		for (const auto& [key, comp] : actorB->getComponentsMap()) {
			if (comp["OnTriggerExit"].isFunction()) {
				try { comp["OnTriggerExit"](comp, colB); }
				catch (luabridge::LuaException const& e) {
					std::cout << "Lua error in OnTriggerExit (B): " << e.what();
				}
			}

		}
	}


}


RigidBody::RigidBody() {
	if (!world) {
		b2Vec2 gravity(0.0f, 9.8f);
		world = new b2World(gravity);
		contactListener = new CollisionDetector();
		world->SetContactListener(contactListener);
	}
}



void RigidBody::step() {
	if (!world) return;
	world->Step(1.0f / 60.0f, 8, 3);
}

void RigidBody::onStart() {
	b2BodyDef tempBody;
	tempBody.position = b2Vec2(x, y);
	if (body_type == "dynamic") {
		tempBody.type = b2_dynamicBody;
	}
	else if (body_type == "static") {
		tempBody.type = b2_staticBody;
	}
	else if (body_type == "kinematic") {
		tempBody.type = b2_kinematicBody;
	}
	tempBody.bullet = precise;
	tempBody.gravityScale = gravity_scale;
	tempBody.angularDamping = angular_friction;
	tempBody.angle = rotation * (b2_pi / 180.0f);
	body = world->CreateBody(&tempBody);
	//Create temp shape
	if (!has_collider && !has_trigger) {
		b2PolygonShape phantom_shape;
		phantom_shape.SetAsBox(width * 0.5f, height * 0.5f);
		b2FixtureDef phantom_fixture_def;
		phantom_fixture_def.shape = &phantom_shape;
		phantom_fixture_def.density = density;
		phantom_fixture_def.isSensor = true;
		b2Fixture* phantomFixture = body->CreateFixture(&phantom_fixture_def);
		return;
	}
	if (has_collider) {
		b2FixtureDef fixture;
		fixture.isSensor = false;
		fixture.density = density;
		fixture.restitution = bounciness;
		fixture.friction = friction;
		fixture.userData.pointer = reinterpret_cast<uintptr_t>(actor);
		if (collider_type == "box") {
			b2PolygonShape my_shape;
			my_shape.SetAsBox(0.5f * width, 0.5f * height);
			fixture.shape = &my_shape;
			b2Fixture* createdFixture = body->CreateFixture(&fixture);
			createdFixture->GetUserData().pointer = reinterpret_cast<uintptr_t>(actor);
		}
		else if (collider_type == "circle") {
			b2CircleShape my_shape;
			my_shape.m_radius = radius;
			fixture.shape = &my_shape;
			body->CreateFixture(&fixture);
		}
	}

	if (has_trigger) {
		b2FixtureDef fixture;
		fixture.isSensor = true;
		fixture.density = density;
		fixture.restitution = bounciness;
		fixture.friction = friction;
		fixture.userData.pointer = reinterpret_cast<uintptr_t>(actor);
		if (trigger_type == "box") {
			b2PolygonShape my_shape;
			my_shape.SetAsBox(0.5f * trigger_width, 0.5f * trigger_height);
			fixture.shape = &my_shape;
			b2Fixture* createdFixture = body->CreateFixture(&fixture);
			createdFixture->GetUserData().pointer = reinterpret_cast<uintptr_t>(actor);
		}
		else if (trigger_type == "circle") {
			b2CircleShape my_shape;
			my_shape.m_radius = trigger_radius;
			fixture.shape = &my_shape;
			body->CreateFixture(&fixture);
		}
	}
}


void RigidBody::OnDestroy() {
	world->DestroyBody(body);
}

void RigidBody::lateUpdate() {
	//step();
}


struct ClosestRaycastCallback : public b2RayCastCallback {
	float minFraction = 1.0f;
	b2Vec2 point;
	b2Vec2 normal;
	bool isTrigger = false;
	ActorDB* actor = nullptr;

	float ReportFixture(b2Fixture* fixture, const b2Vec2& p, const b2Vec2& n, float f) override {
		auto* a = reinterpret_cast<ActorDB*>(fixture->GetUserData().pointer);
		if (!a) return -1.0f;

		if (f < minFraction) {
			minFraction = f;
			point = p;
			normal = n;
			isTrigger = fixture->IsSensor();
			actor = a;
		}
		return f;
	}
};

luabridge::LuaRef RigidBody::Raycast(b2Vec2 pos, b2Vec2 dir, float dist) {
	lua_State* L = ActorDB::getLuaState();
	if (!world || dist <= 0.0f) return luabridge::LuaRef(L);

	dir.Normalize();
	b2Vec2 end = pos + dist * dir;

	ClosestRaycastCallback callback;
	world->RayCast(&callback, pos, end);

	if (!callback.actor) return luabridge::LuaRef(L);

	luabridge::LuaRef result = luabridge::newTable(L);
	result["actor"] = callback.actor;
	result["point"] = callback.point;
	result["normal"] = callback.normal;
	result["is_trigger"] = callback.isTrigger;
	return result;
}

struct AllRaycastCallback : public b2RayCastCallback {
	struct Hit {
		ActorDB* actor;
		b2Vec2 point;
		b2Vec2 normal;
		float fraction;
		bool isTrigger;
	};

	std::vector<Hit> hits;

	float ReportFixture(b2Fixture* fixture, const b2Vec2& p, const b2Vec2& n, float f) override {
		auto* a = reinterpret_cast<ActorDB*>(fixture->GetUserData().pointer);
		if (!a) return -1.0f;

		hits.push_back({ a, p, n, f, fixture->IsSensor() });
		return 1.0f;
	}
};

luabridge::LuaRef RigidBody::RaycastAll(b2Vec2 pos, b2Vec2 dir, float dist) {
	lua_State* L = ActorDB::getLuaState();
	if (!world || dist <= 0.0f) return luabridge::LuaRef(L);

	dir.Normalize();
	b2Vec2 end = pos + dist * dir;

	AllRaycastCallback callback;
	world->RayCast(&callback, pos, end);

	// Sort by distance
	std::sort(callback.hits.begin(), callback.hits.end(), [](const auto& a, const auto& b) {
		return a.fraction < b.fraction;
		});

	luabridge::LuaRef results = luabridge::newTable(L);
	int index = 1;
	for (const auto& h : callback.hits) {
		luabridge::LuaRef hit = luabridge::newTable(L);
		hit["actor"] = h.actor;
		hit["point"] = h.point;
		hit["normal"] = h.normal;
		hit["is_trigger"] = h.isTrigger;
		results[index++] = hit;
	}
	return results;
}