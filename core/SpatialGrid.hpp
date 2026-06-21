#pragma once
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <vector>
#include <unordered_map>
#include <shared_mutex>

namespace Core {
class SpatialGrid {
public:
    explicit SpatialGrid(float cellSize);

    // Hash function combining 3D grid cell indices into a single key
    struct CellHash {
        std::size_t operator()(const glm::ivec3& cell) const noexcept {
            return (std::hash<int>()(cell.x) ^ (std::hash<int>()(cell.y) << 1)) ^ (std::hash<int>()(cell.z) << 2);
        }
    };

    void Clear();

    void Insert(entt::entity entity, const glm::vec3& position);

    std::vector<entt::entity> GetNeighbors(const glm::vec3& position, float radius);

private:
    glm::ivec3 GetCellIndex(const glm::vec3& position) const;

    float m_CellSize;
    std::unordered_map<glm::ivec3, std::vector<entt::entity>, CellHash> m_Cells;
    std::shared_mutex m_GridMutex;
};
