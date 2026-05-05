#pragma once
#include <cstdint>

struct SimulationConfig{
    uint32_t worldHeight = 2000;
    uint32_t worldWidth  = 2000;
    uint32_t worldDepth  = 2000;
    uint32_t maxAgents   = 5000; 
};