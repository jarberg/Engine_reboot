#include "engine/public/core/entity.h"


Entity::Entity(entt::entity s_entity_handle, World* s_scene)
	: entity_handle(s_entity_handle), world(s_scene)
{}

Entity::Entity()
{}