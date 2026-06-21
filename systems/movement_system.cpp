#include "core/movement_system.h"

namespace Systems 
{
    void MovementSystem::UpdateRange(entt::registry& registry, float deltaTime, size_t rangeStart, size_t rangeEnd)
    {
        auto view = registry.view<Components::Transform, Components::Velocity>();
        auto* entities = view.data();
    
        for (size_t i = rangeStart; i < rangeEnd; ++i) 
        {
            entt::entity entity = entities[i];
            auto& transform      = view.get<Components::Transform>(entity);
            const auto& velocity = view.get<Components::Velocity>(entity);
    
            // Standard kinematic integration step
            transform.Position += velocity.Linear * deltaTime;
        }
    }
}
