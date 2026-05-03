#include <spdlog/spdlog.h>
#include "core/simulation.hpp"


int main() {
    spdlog::info("=== AetherSim Starting ===");

    SimulationConfig config{};
    config.maxAgents = 5000;
    Simulation sim(config);

    spdlog::info("Running simulation for 300 ticks...");
    sim.run(300);

    spdlog::info("Simulation finished");
    spdlog::info("Final agent count: {}", sim.getAgentCount());
    spdlog::info("Average tick time: {:.2f} ms", sim.getAverageTickTimeMs());

    return 0;
}