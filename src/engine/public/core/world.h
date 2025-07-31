#pragma once

#include "entt.hpp"
#include "core/components.h"


class Entity;

class World
{
private:
	entt::registry registry_;

	void print_system() {};
	void move_system(float dt) {};

public:
	World();
	~World();

	Entity create_entity(std::string tag);

	entt::registry& get_registry();

	Entity get_entity_by_id(uint32_t id);
	void update(float dt);

	friend class Entity;
};