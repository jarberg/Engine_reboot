#include "core/entity.h"
#include "core/world.h"


Entity::Entity(entt::entity s_entity_handle, World* s_scene)
	: entity_handle(s_entity_handle), world(s_scene)
{}

Entity::Entity()
{}
