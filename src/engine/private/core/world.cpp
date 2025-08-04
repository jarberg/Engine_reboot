
#include <stdio.h>
#include <iostream>

#include "core/world.h"
#include "core/components.h"
#include "core/entity.h"


std::chrono::duration<float> World::deltaTime = std::chrono::duration<float>(0.1f);

World::World()
{} 

World::~World()
{}

Entity World::create_entity(std::string tag){
    Entity entity(registry_.create(), this);
    this->add_tag<Tag>(entity, tag);
    this->add_component<PositionComponent>(entity, 0.0f, 0.0f, 0.0f);
    this->add_component<UuidComponent>(entity, 50);
    return entity;
}
entt::registry& World::get_registry(){
    return registry_;
}

float World::get_delta_time()
{
    return deltaTime.count();
}

Entity World::get_entity_by_id(uint32_t uuid){
    auto view = registry_.view<UuidComponent>();
    for (auto e : view) {
        if (view.get<UuidComponent>(e).uuid == uuid) {
            return Entity(e, this);
        }
    }
    return Entity(); 
}

void World::update(){
    currentTime = std::chrono::high_resolution_clock::now();

    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    print_system();
    move_system(deltaTime.count());
}