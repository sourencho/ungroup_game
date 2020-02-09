#ifndef WinCondition_hpp
#define WinCondition_hpp

// todo: pull resource defines into ResourceDef
#include "../systems/ResourceController.hpp"

class WinCondition {
  public:
    WinCondition(uint32_t player_id);
    ~WinCondition();

    bool satisfiesCondition(std::array<uint32_t, RESOURCE_TYPE_COUNT> current_resources) const;
    const std::array<uint32_t, RESOURCE_TYPE_COUNT>& getResourceCountsToWin() const;
    void
    setResourceCountToWin(const std::array<uint32_t, RESOURCE_TYPE_COUNT>& resource_counts_to_win);

  private:
    std::array<uint32_t, RESOURCE_TYPE_COUNT> m_resourceCountsToWin;
};

sf::Packet& operator<<(sf::Packet& packet, const std::array<uint32_t, RESOURCE_TYPE_COUNT>& wc);
sf::Packet& operator>>(sf::Packet& packet, std::array<uint32_t, RESOURCE_TYPE_COUNT>& wc);

#endif /* WinCondition_hpp */
