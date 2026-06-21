#include "AISystem.hpp"
#include "../components/Velocity.hpp"
#include "../components/AIAgent.hpp"

namespace Systems
{
  void AISystem::UpdateRange(entt::registry& registry, float deltaTime, size_t rangeStart, size_t rangeEnd) 
  {
      // Thread-local engine prevents cross-thread lock contention on the random device
      thread_local std::mt19937 generator(std::random_device{}());
      std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
  
      auto view = registry.view<Components::AIAgent, Components::Velocity>();
      auto* entities = view.data();
  
      for (size_t i = rangeStart; i < rangeEnd; ++i) 
      {
          entt::entity entity = entities[i];
          auto& velocity    = view.get<Components::Velocity>(entity);
          const auto& agent = view.get<Components::AIAgent>(entity);
  
          // Calculate a 3D wandering unit vector scaled by configuration speed
          velocity.Linear.x = distribution(generator) * agent.Speed;
          velocity.Linear.y = distribution(generator) * agent.Speed;
          velocity.Linear.z = distribution(generator) * agent.Speed;
      }
  }
}
