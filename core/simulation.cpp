#include "core/simulation.hpp"
#include "../components/transform.hpp"
#include "../components/velocity.hpp"
#include "../components/ai_agent.hpp"
#include "../systems/movement_system.hpp"
#include "../systems/ai_system.hpp"
#include <spdlog/spdlog.h>
#include <chrono>


Simulation::Simulation(const SimulationConfig& config)
    : m_config(config)
{
    spawnInitialAgents(config.maxAgents);
    spdlog::info("Simulation initialized with {} agents", config.maxAgents);
}


Simulation::~Simulation() = default;


void Simulation::spawnInitialAgents(uint32_t count)
{
    for (uint32_t i = 0; i < count; ++i)
    {
        auto e = m_registry.create();
        auto& t = m_registry.emplace<Transform>(e);
       
        t.position.x = static_cast<float>(rand() % m_config.worldWidth);
        t.position.y = static_cast<float>(rand() % m_config.worldHeight);
        t.position.z = static_cast<float>(rand() % m_config.worldDepth);

        m_registry.emplace<Velocity>(e);
        m_registry.emplace<AIAgent>(e);
    }
}


entt::entity Simulation::createAgent(glm::vec3 position)
{
    auto e = m_registry.create();
    auto& t = m_registry.emplace<Transform>(e);
    t.position = position;
    
    m_registry.emplace<Velocity>(e);
    m_registry.emplace<AIAgent>(e);

    return e;
}


void Simulation::destroyAgent(entt::entity entity)
{
    if (m_registry.valid(entity)) {
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
        m_ticksRan++;

        m_accumulator -= m_tickRate;
    }
}


void Simulation::tick(float dt)
{
    systems::MovementSystem::update(m_registry, dt);
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