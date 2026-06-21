#include "ai_system.hpp"
#include "../components/velocity.hpp"
#include "../components/ai_agent.hpp"

namespace Systems
{
  void AISystem::UpdateRange(entt::registry& registry, float deltaTime, const std::vector<entt::entity>& entities, size_t startIdx, size_t endIdx) 
  {
    thread_local std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

    auto view = registry.view<Components::AIAgent, Components::Velocity>();

    for (size_t i = startIdx; i < endIdx; ++i) 
    {
        entt::entity entity = entities[i];
        auto& velocity = view.get<Components::Velocity>(entity);
        const auto& agent = view.get<Components::AIAgent>(entity);

        // Per-tick wandering intention modification
        velocity.Linear.x += distribution(generator) * agent.Speed * deltaTime;
        velocity.Linear.y += distribution(generator) * agent.Speed * deltaTime;
        velocity.Linear.z += distribution(generator) * agent.Speed * deltaTime;
        
        // Clamp to max speed
        if (glm::length(velocity.Linear) > agent.Speed)
        {
            velocity.Linear = glm::normalize(velocity.Linear) * agent.Speed;
        }
    }
  }
}
