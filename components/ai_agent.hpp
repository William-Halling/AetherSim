#pragma once

namespace Components
{
        /**
         * @struct AIAgent
         * @brief Configuration and state for the AI logic processing.
         */
    struct AIAgent {
        float Speed{80.0f};
        float PerceptionRadius{150.0f};
    };
}
