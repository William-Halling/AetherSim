#include "core/movement_system.h"

namespace systems {

void MovementSystem::update(entt::registry& registry, float deltaTime, size_t rangeStart, size_t rangeEnd)
{
    auto view = registry.view<Components::Transform, Components::Velocity>();

    auto it    = view.begin() + rangeStart;
    auto itEnd = view.begin() + rangeEnd;


    for (; it != itEnd; ++it) 
    {
        const auto entity    = *it;
        auto& transform      = view.get<Components::Transform>(entity);
        const auto& velocity = view.get<Components::Velocity>(entity);

        transform.Position += velocity.Linear * deltaTime;
    }
}
}