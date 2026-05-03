#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <cstdint>

struct SimulationConfig {
    uint32_t worldWidth{2000};
    uint32_t worldHeight{2000};
    uint32_t worldLength{2000};
    uint32_t maxAgents{5000};
};


class Simulation {
public:
    explicit Simulation(const SimulationConfig& config = {});
    ~Simulation();

    void update(float deltaTime);
    void run(uint32_t numTicks);

    entt::entity createAgent(glm::vec3 position);
    void destroyAgent(entt::entity entity);

    [[nodiscard]] size_t getAgentCount() const;
    [[nodiscard]] float getAverageTickTimeMs() const;

private:
    entt::registry m_registry;
    SimulationConfig m_config;

    float m_accumulator = 0.0f;
    const float m_tickRate = 1.0f / 60.0f;

    double m_totalTickTimeMs = 0.0;
    uint32_t m_ticksRan = 0;

    void spawnInitialAgents(uint32_t count);
    void tick(float dt);
};