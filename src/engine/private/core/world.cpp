
#include <stdio.h>
#include <iostream>
#include <math.h>

#include "core/world.h"
#include "core/components.h"
#include "core/entity.h"


std::chrono::duration<float> World::deltaTime = std::chrono::duration<float>(0.1f);
World* World::tlsCurrent = nullptr;

World::World(){
    this->makeCurrent();
} 

World::~World(){
}

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

void World::move_system(float dt){

    auto view = get_registry().view<CharacterComponent, PositionComponent, VelocityComponent>();
    
    view.each([dt](auto& character, auto& position, auto& velocity){
        position.x += velocity.x * dt;
        // position.y += std::max(0.0f, velocity.y * dt);
        position.z += velocity.z * dt;
		});

};

void World::update(){
    currentTime = std::chrono::high_resolution_clock::now();

    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    print_system();
    move_system(deltaTime.count());
}


bool checkCollision(const BoxColliderComponent& a, const BoxColliderComponent& b) {
    return (std::abs(a.halfExtents.X - b.halfExtents.X) <= (a.halfExtents.Y + b.halfExtents.Y)) &&
        (std::abs(a.halfExtents.Y - b.halfExtents.Y) <= (a.halfExtents.Z + b.halfExtents.Z));
}

void checkAllCollisions(entt::registry& registry) {
    auto view = registry.view<BoxColliderComponent, PositionComponent>();

    for (auto e1 : view) {
        auto& c1 = view.get<BoxColliderComponent>(e1);

        for (auto e2 : view) {
            if (e1 == e2) continue; // skip self

            auto& c2 = view.get<BoxColliderComponent>(e2);

            if (checkCollision(c1, c2)) {
                std::cout << "Collision between "
                    << int(e1) << " and " << int(e2) << "\n";
            }
        }
    }
}