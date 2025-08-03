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


    template<typename T, typename... Args>
    T& add_component(Entity entity, Args&&... args) {
        return registry_.emplace<T>(entity.get_handle(), std::forward<Args>(args)...);
    }

    template<typename T>
    T& get_component(Entity entity) {
        return registry_.get<T>(entity.get_handle());
    }

    template<typename T>
    bool has_component(Entity entity) {
        return registry_.all_of<T>(entity.get_handle());
    }

    template<typename T>
    void remove_component(Entity entity) {
        registry_.remove<T>(entity.get_handle());
    }

    template<typename T, typename... Args>
    T& add_tag(Entity entity, Args&&... args)
    {
        return registry_.emplace<T>(entity.get_handle(), std::forward<Args>(args)...);
    }
};
