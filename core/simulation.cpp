#include "core/simulation.hpp"
#include "core/job_system.hpp"
#include "../components/transform.hpp"
#include "../components/velocity.hpp"
#include "../components/ai_agent.hpp"
#include "../systems/movement_system.hpp"
#include "../systems/ai_system.hpp"
#include <spdlog/spdlog.h>
#include <chrono>


Simulation::Simulation(const SimulationConfig& config)
    : m_config(config), 
    m_jobSystem(std::make_unique<JobSystem<(0))
{
    spawnInitialAgents(config.maxAgents);

    spdlog::info("Simulation initialized with {} agents", config.maxAgents, m_jobSystem->getThreadCount());
}


Simulation::~Simulation() = default;


void Simulation::spawnInitialAgents(uint32_t count)
{
    for (uint32_t i = 0; i < count; ++i) {

        auto entity = m_registry.create();

        auto& transform      = m_registry.emplace<Transform>(entity);
        transform.position.x = static_cast<float>(rand() % m_config.worldWidth);
        transform.position.y = static_cast<float>(rand() % m_config.worldHeight);
        transform.position.z = static_cast<float>(rand() % m_config.worldDepth);

        m_registry.emplace<Velocity>(entity);
        m_registry.emplace<AIAgent>(entity);
    }
}


entt::entity Simulation::createAgent(glm::vec3 position)
{
    auto entity        = m_registry.create();
    auto& transform    = m_registry.emplace<Transform>(entity);
    transform.position = position;
    
    m_registry.emplace<Velocity>(entity);
    m_registry.emplace<AIAgent>(entity);

    return entity;
}


void Simulation::destroyAgent(entt::entity entity)
{
    if (m_registry.valid(entity)){

        m_registry.destroy(entity);
    }
}


void Simulation::update(float deltaTime)
{
    m_accumulator += deltaTime;

    while (m_accumulator >= m_tickRate) {

        auto start = std::chrono::high_resolution_clock::now();

        tick(m_tickRate);

        auto end = std::chrono::high_resolution_clock::now();
        m_totalTickTimeMs += std::chrono::duration<double, std::milli>(end - start).count();
        m_tickCount++;

        m_accumulator -= m_tickRate;
    }
}


void Simulation::tick(float dt)
{
    const uint32_t jobCount = static_cast<uint32_t>(m_jobSystem->getThreadCount());

    std::vector<JobSystem::Job> movementJobs;
    movementJobs.reserve(jobCount);
    for (uint32_t i = 0; i < jobCount; ++i) {
        movementJobs.emplace_back([this, dt, i, jobCount]() {
            updateMovementChunk(i, jobCount, dt);
        });
    }

    std::vector<JobSystem::Job> aiJobs;
    aiJobs.reserve(jobCount);
    for (uint32_t i = 0; i < jobCount; ++i) {
        aiJobs.emplace_back([this, dt, i, jobCount]() {
            updateAIChunk(i, jobCount, dt);
        });
    }

        // execute in parallel threads
    m_jobSystem->scheduleAndWait(movementJobs);
    m_jobSystem->scheduleAndWait(aiJobs);
}


void Simulation::updateMovementChunk(uint32_t jobIndex, uint32_t totalJobs, float dt)
{
    systems::MovementSystem::update(m_registry, dt);
}


void Simulation::updateAIChunk(uint32_t jobIndex, uint32_t totalJobs, float dt)
{
    systems::AISystem::update(m_registry, dt);
}


void Simulation::run(uint32_t numTicks)
{
    for (uint32_t i = 0; i < numTicks; ++i) {
        update(1.0f / 60.0f);
    }
}


size_t Simulation::getAgentCount() const
{
    return m_registry.size();
}


float Simulation::getAverageTickTimeMs() const
{
    return m_ticksRan > 0 ? static_cast<float>(m_totalTickTimeMs / m_ticksRan) : 0.0f;
}