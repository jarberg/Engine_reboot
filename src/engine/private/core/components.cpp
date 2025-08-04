#include "core/components.h"


void CharacterComponent::inputEvent(Entity owner, std::shared_ptr<KeyPressedEvent> e)
{
	World* world = owner.get_world();
	float dt = world->get_delta_time();
    if (!world->has_component<PositionComponent>(owner)) {
        return;
    }

    PositionComponent& posComp = world->get_component<PositionComponent>(owner);
    switch (e->key){
        case KeyCode::Left:
            posComp.x -= 1.0f* dt;
            break;
        case KeyCode::Right:
            posComp.x += 1.0f * dt;
            break;
        case KeyCode::Up:
            posComp.y += 1.0f * dt;
            break;
        case KeyCode::Down:
            posComp.y -= 1.0f * dt;
            break;
    }
}
