#include <spdlog/spdlog.h>
#include "core/Simulation.hpp"


int main() {
    spdlog::info("=== AetherSim Starting ===");

    Core::SimulationConfig config{};
    config.maxAgents = 5000;
    Core::Simulation sim(config);

    spdlog::info("Running simulation for 300 ticks...");
    sim.Run(300); // Fixed naming casing alignment

    spdlog::info("Simulation finished");
    spdlog::info("Final agent count: {}", sim.GetAgentCount()); // Fixed naming casing alignment
    spdlog::info("Average tick time: {:.2f} ms", sim.GetAverageTickTimeMs()); // Fixed naming casing alignment

    return 0;
}
