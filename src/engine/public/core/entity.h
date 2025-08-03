#pragma once

#include "entt.hpp"

class World;

class Entity
{
private:

	entt::entity entity_handle = entt::null;
	World* world = nullptr;

public:
	Entity();

	Entity(entt::entity s_entity_handle, World* s_World);

	inline entt::entity get_handle() { return entity_handle; }

	inline World* get_world() { return world; }

};
