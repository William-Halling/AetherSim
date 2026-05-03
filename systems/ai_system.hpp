#pragma once
#include <entt/entt.hpp>

namespace systems{
  
class AISystem {
public:
    static void update(entt::registry& registry, float dt);
};

}