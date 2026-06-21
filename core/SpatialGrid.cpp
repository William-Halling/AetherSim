#include "SpatialGrid.hpp"

namespace Core 
{
    // High-performance hash combining 3D cell coordinates to minimize hash collisions
  std::size_t SpatialGrid::CellHash::operator()(const glm::ivec3& cell) const noexcept {
      return (std::hash<int>()(cell.x) ^ (std::hash<int>()(cell.y) << 1)) ^ (std::hash<int>()(cell.z) << 2);
  }
  
  SpatialGrid::SpatialGrid(float cellSize) 
      : m_CellSize(cellSize) {}
  
  void SpatialGrid::Clear() {
        // Exclusive lock required to modify the underlying collection
      std::unique_lock<std::shared_mutex> lock(m_GridMutex);
      m_Cells.clear();
  }
  
  void SpatialGrid::Insert(entt::entity entity, const glm::vec3& position) {
      glm::ivec3 cellKey = GetCellIndex(position);
      
        // Exclusive lock required for thread-safe insertions
      std::unique_lock<std::shared_mutex> lock(m_GridMutex);
      m_Cells[cellKey].push_back(entity);
  }
  
  std::vector<entt::entity> SpatialGrid::GetNeighbors(const glm::vec3& position, float radius) {
      std::vector<entt::entity> neighbors;
      
      glm::ivec3 minCell = GetCellIndex(position - glm::vec3(radius));
      glm::ivec3 maxCell = GetCellIndex(position + glm::vec3(radius));
  
      // Shared lock allows concurrent reading across multiple AISystem threads
      std::shared_lock<std::shared_mutex> lock(m_GridMutex);
      
      // Pre-reserve a baseline size to avoid vector reallocation spikes on the heap
      neighbors.reserve(32); 
  
      for (int x = minCell.x; x <= maxCell.x; ++x) {
          for (int y = minCell.y; y <= maxCell.y; ++y) {
              for (int z = minCell.z; z <= maxCell.z; ++z) {
                  auto it = m_Cells.find({x, y, z});
                  if (it != m_Cells.end()) {
                      neighbors.insert(neighbors.end(), it->second.begin(), it->second.end());
                  }
              }
          }
      }
      return neighbors;
  }
  
  glm::ivec3 SpatialGrid::GetCellIndex(const glm::vec3& position) const {
      return {
          static_cast<int>(std::floor(position.x / m_CellSize)),
          static_cast<int>(std::floor(position.y / m_CellSize)),
          static_cast<int>(std::floor(position.z / m_CellSize))
      };
  }

}
