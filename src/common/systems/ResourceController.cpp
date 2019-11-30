#include "ResourceController.hpp"

#include <iostream>
#include <limits>

void ResourceController::init(uint32_t id) {
    if (m_resourceCounts.count(id) == 0) {
        m_resourceCounts[id] = {0, 0, 0, 0};
    }
}

std::array<uint32_t, RESOURCE_TYPE_COUNT> ResourceController::get(uint32_t id) {
    init(id);

    return m_resourceCounts[id];
}

uint32_t ResourceController::get(uint32_t id, ResourceType resource_type) {
    init(id);

    return m_resourceCounts[id][resource_type];
}

void ResourceController::set(uint32_t id, ResourceType resource_type, uint32_t count) {
    init(id);

    m_resourceCounts[id][resource_type] = count;
}

std::pair<uint32_t, uint32_t> ResourceController::add(uint32_t id, ResourceType resource_type,
                                                      uint32_t count) {
    init(id);

    uint32_t current_count = m_resourceCounts[id][resource_type];
    uint32_t add_count = count;

    if (current_count > std::numeric_limits<uint32_t>::max() - count) {
        add_count = std::numeric_limits<uint32_t>::max() - current_count;
    }

    uint32_t new_count = current_count + add_count;
    m_resourceCounts[id][resource_type] = new_count;

    return std::make_pair(add_count, new_count);
}

std::pair<uint32_t, uint32_t> ResourceController::subtract(uint32_t id, ResourceType resource_type,
                                                           uint32_t count) {
    init(id);

    uint32_t current_count = m_resourceCounts[id][resource_type];
    uint32_t subtract_count = count;
    if (current_count < count) {
        subtract_count = current_count;
    }

    uint32_t new_count = current_count - subtract_count;
    m_resourceCounts[id][resource_type] = new_count;

    return std::make_pair(subtract_count, new_count);
}

std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>
ResourceController::move(uint32_t from, uint32_t to, ResourceType resource_type, uint32_t count) {
    init(to);
    init(from);

    uint32_t subtract_count, sender_count, add_count, reciever_count;
    std::tie(subtract_count, sender_count) = subtract(from, resource_type, count);
    std::tie(add_count, reciever_count) = add(to, resource_type, subtract_count);

    return std::make_tuple(subtract_count, sender_count, add_count, reciever_count);
}

/* Network utilities */

ResourceControllerUpdate ResourceController::getUpdate() {
    std::vector<IdAndCounts> id_and_counts_list;

    for (auto kv : m_resourceCounts) {
        IdAndCounts id_and_counts;
        id_and_counts.id = kv.first;
        id_and_counts.counts = kv.second;
        id_and_counts_list.push_back(id_and_counts);
    }

    ResourceControllerUpdate rcu = {
        .id_and_counts_list_size = static_cast<sf::Uint32>(id_and_counts_list.size()),
        .id_and_counts_list = id_and_counts_list,
    };

    return rcu;
}

void ResourceController::applyUpdate(ResourceControllerUpdate rcu) {
    m_resourceCounts.clear();

    for (auto& ic : rcu.id_and_counts_list) {
        m_resourceCounts[ic.id] = ic.counts;
    }
}

sf::Packet& operator<<(sf::Packet& packet, const IdAndCounts& ic) {
    packet << ic.id;
    for (auto& count : ic.counts) {
        packet << count;
    }
    return packet;
}

sf::Packet& operator>>(sf::Packet& packet, IdAndCounts& ic) {
    packet >> ic.id;

    std::array<uint32_t, RESOURCE_TYPE_COUNT> counts;
    for (size_t i = 0; i < RESOURCE_TYPE_COUNT; i++) {
        sf::Uint32 count;
        packet >> count;
        counts[i] = static_cast<uint32_t>(count);
    }
    ic.counts = counts;
    return packet;
}

sf::Packet& operator<<(sf::Packet& packet, const ResourceControllerUpdate& rcu) {
    packet << rcu.id_and_counts_list_size;
    for (auto& id_and_counts : rcu.id_and_counts_list) {
        packet << id_and_counts;
    }

    return packet;
}

sf::Packet& operator>>(sf::Packet& packet, ResourceControllerUpdate& rcu) {
    packet >> rcu.id_and_counts_list_size;

    std::vector<IdAndCounts> id_and_counts_list;
    id_and_counts_list.reserve(rcu.id_and_counts_list_size);
    for (int i = 0; i < rcu.id_and_counts_list_size; ++i) {
        IdAndCounts id_and_counts;
        packet >> id_and_counts;
        id_and_counts_list.push_back(id_and_counts);
    }
    rcu.id_and_counts_list = id_and_counts_list;

    return packet;
}