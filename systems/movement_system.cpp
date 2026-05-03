#include "core/movement_system.h"

namespace systems {

void MovementSystem::update(entt::registry& registry, float deltaT)
{
    auto view = registry.view<Transform, Velocity>();

    for(auto entity : view)
    {
        auto& pos = view.get<Transform>(entity).position;

        auto& vel = view.get<Velocity>(entity);

        pos += vel.value * deltaT;
    }
}

}