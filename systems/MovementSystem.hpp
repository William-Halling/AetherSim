#pragma once
#include <entt/entt.hpp>

namespace Systems {

    /**
     * @class MovementSystem
     * @brief Handles kinematic integration of velocity into position.
     */
class MovementSystem {
public:
        /**
         * @brief Integrates velocity into transform positions for a specific range of entities.
         * @param registry The ECS registry containing components.
         * @param deltaTime Elapsed time for the current tick.
         * @param rangeStart The starting index in the view.
         * @param rangeEnd The ending index in the view.
         */
    static void UpdateRange(entt::registry& registry, float deltaTime, const std::vector<entt::entity>& entities, size_t startIdx, size_t endIdx);
};
}
