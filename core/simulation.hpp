#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <cstdint>
#include <memory>
#include <vector>
#include "SimulationConfig.hpp"
#include "core/TaskScheduler.hpp"

namespace Core {

    /**
     * @class Simulation
     * @brief Manages the high-level life cycle, entity registry, and parallelized systems.
     * 
     * The Simulation class implements a fixed-timestep update loop, ensuring deterministic 
     * physics and AI logic regardless of variable frame rates.
     */
class Simulation {
public:
        /**
         * @brief Constructs the simulation environment.
         * @param config Configuration parameters for world bounds and agent counts.
         */
    explicit Simulation(const SimulationConfig& config = {});
    

        /** @brief Destructor handles graceful cleanup of the TaskScheduler and Registry. */
    ~Simulation();


        // Deleted to prevent accidental duplication of heavy system resources
    Simulation(const Simulation&) = delete;
    Simulation& operator=(const Simulation&) = delete;


        /**
         * @brief Updates the simulation accumulator and executes fixed-rate ticks.
         * @param deltaTimeSeconds Elapsed time since the last frame in seconds.
         */
    void Update(float deltaTimeSeconds);


        /**
         * @brief Forces the simulation to run for a specific number of ticks.
         * @param tickCount Number of simulation steps to execute.
         */
    void Run(uint32_t tickCount);


        /**
         * @brief Creates a new agent entity at the specified position.
         * @param position Initial 3D coordinates for the agent.
         * @return The resulting entt::entity handle.
         */
    entt::entity CreateAgent(const glm::vec3& position);


        /**
         * @brief Safely removes an agent from the registry.
         * @param entity Handle of the agent to destroy.
         */
    void DestroyAgent(entt::entity entity);


        /** @return Total number of active agents in the registry. */
    [[nodiscard]] size_t GetAgentCount() const noexcept;


        /** @return The calculated average time taken to process a single tick in milliseconds. */
    [[nodiscard]] float GetAverageTickTimeMs() const noexcept;

private:

        /** @brief Populates the world with agents based on the initial configuration. */
    void SpawnInitialAgents(uint32_t count);

    
        /** @brief Executes a single logic step (AI and Movement). */
    void OnTick(float tickRate);


        /** @brief Dispatches movement logic across available worker threads. */
    void DispatchMovement(float deltaTime);


        /** @brief Dispatches AI logic across available worker threads. */
    void DispatchAI(float deltaTime);


private:
    SimulationConfig           m_Config;
    entt::registry             m_Registry;
    std::unique_ptr<TaskScheduler> m_TaskScheduler;

    float  m_TimeAccumulator{0.0f};
    const float m_FixedTickRate{1.0f / 60.0f};

    double   m_TotalTickTimeMs{0.0};
    uint32_t m_TicksExecutedCount{0};
};

}