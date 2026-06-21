#pragma once
#include <glm/glm.hpp>

namespace Components {

        /**
         * @struct Velocity
         * @brief Represents the linear physical movement vector of an entity.
         */
    struct Velocity{

        glm::vec3 Linear{0.0f};
    };
}
