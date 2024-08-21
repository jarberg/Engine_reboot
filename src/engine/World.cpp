
#include <stdio.h>
#include <iostream>

#include "engine/World.h"
#include "engine/Entity.h"


World::World()
{} 

World::~World()
{}

Entity World::create_entity(std::string tag)
{
    Entity entity(registry_.create(), this);
    entity.add_tag<engine::Tag>(tag);
    return entity;
}
entt::registry& World::get_registry()
{
    return registry_;
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