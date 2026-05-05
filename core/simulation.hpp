#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <cstdint>
#include <memory>
#include "simulation_config.hpp"


class JobSystem;


class Simulation {
public:
    explicit Simulation(const SimulationConfig& config = {});
    ~Simulation();

    void update(float deltaTime);
    void run(uint32_t numTicks);

    entt::entity createAgent(glm::vec3 position);
    void destroyAgent(entt::entity entity);

    [[nodiscard]] size_t getAgentCount() const noexcept;
    [[nodiscard]] float getAverageTickTimeMs() const noexcept;

private:
    void spawnInitialAgents(uint32_t count);
    void tick(float dt);


        ///parallel updates
    void updateMovementChunk(uint32_t jobIndex, uint32_t totalJobs, float deltaT);
    void updateAIChunk(uint32_t jobIndex, uint32_t totalJobs, float deltaT);


    SimulationConfig m_config;
    entt::registry   m_registry;
    std::unique_ptr<JobSystem> m_JobSystem;


    float m_accumulator      = 0.0f;
    const float m_tickRate   = 1.0f / 60.0f;

    double m_totalTickTimeMs = 0.0;
    uint32_t m_tickCount     = 0;
};