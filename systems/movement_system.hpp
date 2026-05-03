#pragma once
#include <entt/entt.hpp>

namespace systems{
  
    class MovementSystem
    {
        public:
            static void update(entt::registry& registry, float dt);
    };
}