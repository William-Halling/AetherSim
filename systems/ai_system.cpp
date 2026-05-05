#include "ai_system.hpp"
#include "../components/velocity.hpp"
#include "../components/ai_agent.hpp"

namespace Systems{
  
    void AISystem::ComputeBehaviors(entt::registry& registry, float deltaTime, size_t rangeStart, size_t rangeEnd) 
    {
        
        thread_local std::mt19937 generator(std::random_device{}());
        std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
    
        auto view = registry.view<Components::AIAgent, Components::Velocity>();
        
        auto it    = view.begin() + rangeStart;
        auto itEnd = view.begin() + rangeEnd;
    

        for (; it != itEnd; ++it) 
        {
            const auto entity = *it;
            auto& velocity    = view.get<Components::Velocity>(entity);
            const auto& agent = view.get<Components::AIAgent>(entity);
    
                // Simple wandering logic scaled by agent speed
            velocity.Linear.x = distribution(generator) * agent.Speed;
            velocity.Linear.y = distribution(generator) * agent.Speed;
        }
    }
}