#include "Simulation.hpp"
#include <spdlog/spdlog.h>
#include <chrono>
#include <random>
#include "../components/Transform.hpp"
#include "../components/Velocity.hpp"
#include "../components/AIAgent.hpp"
#include "../systems/MovementSystem.hpp"
#include "../systems/AISystem.hpp"

namespace Core 
{

Simulation::Simulation(const SimulationConfig& config)
    : m_Config(config)
    , m_TaskScheduler(std::make_unique<TaskScheduler>(0))
{
    SpawnInitialAgents(m_Config.maxAgents);

    spdlog::info("Simulation initialized. Agents: {} | Threads: {}", m_Config.maxAgents, m_TaskScheduler->GetThreadCount());
}


Simulation::~Simulation() = default;


void Simulation::SpawnInitialAgents(uint32_t count) 
{
    // Thread-safe random initialization
    std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<float> distWidth(0.0f, (float)m_Config.worldWidth);
    std::uniform_real_distribution<float> distHeight(0.0f, (float)m_Config.worldHeight);
    std::uniform_real_distribution<float> distDepth(0.0f, (float)m_Config.worldDepth);

    for (uint32_t i = 0; i < count; ++i) 
    {
        CreateAgent({ distWidth(generator), distHeight(generator), distDepth(generator) });
    }
}

entt::entity Simulation::CreateAgent(const glm::vec3& position) 
{
    auto entity = m_Registry.create();
    
    auto& transform = m_Registry.emplace<Transform>(entity);
    transform.Position = position;

    m_Registry.emplace<Velocity>(entity);
    m_Registry.emplace<AIAgent>(entity);

    return entity;
}


void Simulation::DestroyAgent(entt::entity entity)
{
    if (m_Registry.valid(entity)) {
        m_Registry.destroy(entity);
    }
}


void Simulation::Update(float deltaTimeSeconds) 
{
    m_TimeAccumulator += deltaTimeSeconds;

    while (m_TimeAccumulator >= m_FixedTickRate) {
        const auto start = std::chrono::high_resolution_clock::now();

        OnTick(m_FixedTickRate);

        const auto end = std::chrono::high_resolution_clock::now();
        m_TotalTickTimeMs += std::chrono::duration<double, std::milli>(end - start).count();
        m_TicksExecutedCount++;

        m_TimeAccumulator -= m_FixedTickRate;
    }
}


void Simulation::OnTick(float tickRate)
{

    DispatchAI(tickRate);
    DispatchMovement(tickRate);
}


void Simulation::DispatchAI(float deltaTime) 
{
    auto view = m_Registry.view<Components::AIAgent, Components::Velocity>();
    
    if (view.empty()) 
        return;

        // Cache active entity IDs to linear memory for safe thread index chunking
    std::vector<entt::entity> entities(view.begin(), view.end());
    const size_t agentCount = entities.size();

    const uint32_t threadCount = static_cast<uint32_t>(m_TaskScheduler->GetThreadCount());
    const size_t chunkSize = (agentCount + threadCount - 1) / threadCount;

    std::vector<TaskScheduler::Task> aiTasks;
    aiTasks.reserve(threadCount);

    for (uint32_t i = 0; i < threadCount; ++i) 
    {
        size_t startIdx = i * chunkSize;
        size_t endIdx = std::min(startIdx + chunkSize, agentCount);

        if (startIdx >= agentCount)
            break;

        aiTasks.emplace_back([this, deltaTime, entities, startIdx, endIdx]()
        {
            Systems::AISystem::UpdateRange(m_Registry, deltaTime, entities, startIdx, endIdx);
        });
    }

    m_TaskScheduler->DispatchAndWait(aiTasks);
}


void Simulation::DispatchMovement(float deltaTime)
{
    auto view = m_Registry.view<Components::Transform, Components::Velocity>();
    
    if (view.empty()) 
    {
        return;
    }

    std::vector<entt::entity> entities(view.begin(), view.end());
    const size_t agentCount = entities.size();

    const uint32_t threadCount = static_cast<uint32_t>(m_TaskScheduler->GetThreadCount());
    const size_t chunkSize = (agentCount + threadCount - 1) / threadCount;
    
    std::vector<TaskScheduler::Task> moveTasks;
    moveTasks.reserve(threadCount);


    for (uint32_t i = 0; i < threadCount; ++i) 
    {
        size_t startIdx = i * chunkSize;
        size_t endIdx = std::min(startIdx + chunkSize, agentCount);

        if (startIdx >= agentCount) 
        {
            break;
        }

        moveTasks.emplace_back([this, deltaTime, startIdx, endIdx]()
        {
            Systems::MovementSystem::UpdateRange(m_Registry, deltaTime, startIdx, endIdx);
        });
    }

    m_TaskScheduler->DispatchAndWait(moveTasks);
}


void Simulation::Run(uint32_t tickCount)
{
    for (uint32_t i = 0; i < tickCount; ++i) {
        Update(m_FixedTickRate);
    }
}


size_t Simulation::GetAgentCount() const noexcept
{
    return m_Registry.size();
}


float Simulation::GetAverageTickTimeMs() const noexcept 
{
    return m_TicksExecutedCount > 0 ? static_cast<float>(m_TotalTickTimeMs / m_TicksExecutedCount) : 0.0f;
}

} // namespace Core
