#pragma once
#include <glm/glm.hpp>

namespace Components {

        /**
         * @struct Transform
         * @brief Represents the spatial state of an entity in 3D space.
         */
    struct Transform {
        glm::vec3 Position{0.0f};
        float Rotation{0.0f};
    };
}