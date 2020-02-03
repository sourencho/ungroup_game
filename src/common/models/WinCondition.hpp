#ifndef WinCondition_hpp
#define WinCondition_hpp

// todo: pull resource defines into ResourceDef
#include "../systems/ResourceController.hpp"

const size_t WIN_CONDITION_RESOURCE_TOTAL = 20;

class WinCondition {
  public:
    WinCondition();
    ~WinCondition();

    bool satisfiesCondition(std::array<uint32_t, RESOURCE_TYPE_COUNT> current_resources) const;
    const std::array<uint32_t, RESOURCE_TYPE_COUNT>& getResourceCountsToWin() const;
    void setResourceCountToWin(ResourceType resource_type, uint32_t count);

  private:
    std::array<uint32_t, RESOURCE_TYPE_COUNT> m_resourceCountsToWin;
};

sf::Packet& operator<<(sf::Packet& packet, const WinCondition& wc);
sf::Packet& operator>>(sf::Packet& packet, WinCondition& wc);

#endif /* WinCondition_hpp */
