#include "core/movement_system.h"

namespace systems 
{
    void MovementSystem::UpdateRange(entt::registry& registry, float deltaTime, const std::vector<entt::entity>& entities, size_t startIdx, size_t endIdx) 
    {
        auto view = registry.view<Components::Transform, Components::Velocity>();

        for (size_t i = startIdx; i < endIdx; ++i) 
        {
            entt::entity entity = entities[i];
            auto& transform = view.get<Components::Transform>(entity);
            const auto& velocity = view.get<Components::Velocity>(entity);

            transform.Position += velocity.Linear * deltaTime;
        }
    }
}
