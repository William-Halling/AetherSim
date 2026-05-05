#pragma once
#include <entt/entt.hpp>

namespace Systems {

    /**
     * @class AISystem
     * @brief Process AI behaviors and updates entity intentions.
     */
class AISystem {
public:
        /**
         * @brief Computes new velocity vectors for agents based on internal logic.
         * @note Uses thread-local random generation to prevent mutex contention.
         */
    static void ComputeBehaviors(entt::registry& registry, float deltaTime, size_t rangeStart, size_t rangeEnd);
};
}