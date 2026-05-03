#include "ai_system.hpp"
#include "../components/velocity.hpp"
#include "../components/ai_agent.hpp"

namespace systems{
  
void AISystem::update(entt::registry& registry, float dt)
{
    auto view = registry.view<AIAgent, Velocity>();
    for (auto entity : view)
    {
        auto& vel = view.get<Velocity>(entity);
        vel.value.x = (rand() % 3 - 1) * 40.0f;
        vel.value.y = (rand() % 3 - 1) * 40.0f;
    }
}

}