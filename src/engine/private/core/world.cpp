
#include <stdio.h>
#include <iostream>

#include "core/world.h"
#include "core/components.h"
#include "core/entity.h"


World::World()
{} 

World::~World()
{}

Entity World::create_entity(std::string tag)
{
    Entity entity(registry_.create(), this);
    entity.add_tag<engine::Tag>(tag);
	entity.add_component<engine::PositionComponent>(0.0f, 0.0f, 0.0f);
    entity.add_component<engine::UuidComponent>(50);
    return entity;
}
entt::registry& World::get_registry()
{
    return registry_;
}

Entity World::get_entity_by_id(uint32_t uuid){
    auto view = registry_.view<engine::UuidComponent>();
    for (auto e : view) {
        if (view.get<engine::UuidComponent>(e).uuid == uuid) {
            return Entity(e, this);
        }
    }
    return Entity(); // Null entity if not found
}
/*
void World::print_system() {
    std::cout << "World update" << std::endl;

    auto view = registry.view<PositionComponent, VelocityComponent, AccelerationComponent>();

    view.each([](auto entity, auto& pos, auto& vel, auto& acc) {
        printf("pos: %lf  %lf\nvel: %lf  %lf\nacc: %lf %lf\n--------------------\n", pos.x, pos.y, vel.x, vel.y, acc.x, acc.y);
        });
}

void World::move_system(float dt)
{
    // Move System
    auto view = registry.view<PositionComponent, VelocityComponent, AccelerationComponent>();

    for (auto entity :view) {
        auto& pos = view.get<PositionComponent>(entity);
        auto& vel = view.get<VelocityComponent>(entity);
        auto& acc = view.get<AccelerationComponent>(entity);

        acc.y = fmax(-9.8f, -9.8f * dt);

        vel.x += acc.x * dt;
        vel.y = fmin(-9.8f, (vel.y+acc.y) * dt); ;

        pos.x += vel.x;
        pos.y += vel.y ;

    }
}
*/
void World::update(float dt)
{
    print_system();
    move_system(dt);
}