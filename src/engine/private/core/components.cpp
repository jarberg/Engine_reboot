#include "core/components.h"


void CharacterComponent::inputEvent(Entity owner, std::shared_ptr<KeyPressedEvent> e)
{
	World* world = owner.get_world();
    
    if (!world->has_component<PositionComponent>(owner)) {
        return;
    }
    PositionComponent& posComp = world->get_component<PositionComponent>(owner);
    switch (e->key){
        case KeyCode::Left:
            posComp.x -= 0.01f;
            break;
        case KeyCode::Right:
            posComp.x += 0.01f;
            break;
        case KeyCode::Up:
            posComp.y += 0.01f;
            break;
        case KeyCode::Down:
            posComp.y -= 0.01f;
            break;
    }
}
