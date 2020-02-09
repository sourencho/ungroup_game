#include "WinCondition.hpp"

#include <iostream>

#include "../util/game_settings.hpp"

WinCondition::~WinCondition() {
}

WinCondition::WinCondition(uint32_t player_id) {
    auto total_resource_requirements = GAME_SETTINGS.TOTAL_RESOURCE_REQUIREMENTS;
    srand(time(NULL) + player_id);

    // define amount of resources required for "primary" required resource
    size_t quantity_required_for_primary_resource =
        (total_resource_requirements / RESOURCE_TYPE_COUNT) * 2;
    total_resource_requirements -= quantity_required_for_primary_resource;
    // pick which resource player will need the most of
    size_t primary_resource = (rand() % RESOURCE_TYPE_COUNT);
    for (size_t i = 0; i < RESOURCE_TYPE_COUNT; i++) {
        if (i == primary_resource) {
            m_resourceCountsToWin[i] = quantity_required_for_primary_resource;
        } else {
            // make all other resources equal split (-1 is to exclude the primary resource)
            m_resourceCountsToWin[i] = total_resource_requirements / (RESOURCE_TYPE_COUNT - 1);
        }
    }
}

bool WinCondition::satisfiesCondition(
    std::array<uint32_t, RESOURCE_TYPE_COUNT> current_resources) const {
    for (size_t i = 0; i < RESOURCE_TYPE_COUNT; i++) {
        if (current_resources[i] < m_resourceCountsToWin[i]) {
            return false;
        }
    }
    return true;
}

const std::array<uint32_t, RESOURCE_TYPE_COUNT>& WinCondition::getResourceCountsToWin() const {
    return m_resourceCountsToWin;
}

void WinCondition::setResourceCountToWin(
    const std::array<uint32_t, RESOURCE_TYPE_COUNT>& resource_counts_to_win) {
    m_resourceCountsToWin = resource_counts_to_win;
}

sf::Packet& operator<<(sf::Packet& packet,
                       const std::array<uint32_t, RESOURCE_TYPE_COUNT>& resource_counts_to_win) {
    for (auto& count : resource_counts_to_win) {
        packet << static_cast<sf::Uint32>(count);
    }
    return packet;
}

sf::Packet& operator>>(sf::Packet& packet,
                       std::array<uint32_t, RESOURCE_TYPE_COUNT>& resource_counts_to_win) {
    for (size_t i = 0; i < RESOURCE_TYPE_COUNT; i++) {
        sf::Uint32 count;
        packet >> count;
        resource_counts_to_win[ResourceType(i)] = count;
    }
    return packet;
}
