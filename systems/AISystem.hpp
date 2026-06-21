#pragma once
#include <entt/entt.hpp>

namespace Systems {

    /**
     * @class AISystem
     * @brief Process AI behaviors and updates entity intentions.
     */
namespace Systems 
{
    class AISystem {
    public:
        static void UpdateRange(entt::registry& registry, float deltaTime, const std::vector<entt::entity>& entities, size_t startIdx, size_t endIdx);
    };
}
