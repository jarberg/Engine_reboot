#pragma once

#include "entt.hpp"

#include "engine/public/core/world.h"

class World;

class Entity
{
private:

	entt::entity entity_handle = entt::null;
	World* world = NULL;

public:	
	Entity();

	Entity(entt::entity s_entity_handle, World* s_World);

	inline entt::entity get_handle() { return entity_handle; }

	template<typename T, typename... Args>
	T& add_component(Args&&... args)
	{
		return world->registry_.emplace<T>(entity_handle, std::forward<Args>(args)...);
	}
	template<typename T, typename... Args>
	T& add_tag(Args&&... args)
	{
		return world->registry_.emplace<T>(entity_handle, std::forward<Args>(args)...);
	}
	template<typename T>
	T& get_component()
	{
		return world->registry_.get<T>(entity_handle);
	}

	template<typename T>
	void remove_component()
	{
		world->registry_.remove<T>(entity_handle);
	}

	template<typename T>
	bool has_component()
	{
		return world->registry_.all_of<T>(entity_handle);
	}
};