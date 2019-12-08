/**
 * Keep track of resource counts for ids.
 */

#ifndef ResourceController_hpp
#define ResourceController_hpp

#include <array>
#include <unordered_map>

#include <SFML/Network.hpp>

enum ResourceType { RED, GREEN, BLUE, YELLOW };
const size_t RESOURCE_TYPE_COUNT = 4;
const std::array<std::string, RESOURCE_TYPE_COUNT> RESOURCE_NAME = {"A", "B", "C", "D"};

/* Network utilities */

struct IdAndCounts {
    sf::Uint32 id;
    std::array<sf::Uint32, RESOURCE_TYPE_COUNT> counts;
};

struct ResourceControllerUpdate {
    sf::Uint32 id_and_counts_list_size;
    std::vector<IdAndCounts> id_and_counts_list;
};

sf::Packet& operator<<(sf::Packet& packet, const IdAndCounts& ic);
sf::Packet& operator>>(sf::Packet& packet, IdAndCounts& ic);

sf::Packet& operator<<(sf::Packet& packet, const ResourceControllerUpdate& rcu);
sf::Packet& operator>>(sf::Packet& packet, ResourceControllerUpdate& rcu);

/* Class */

class ResourceController {
  public:
    ResourceController(){};
    ~ResourceController(){};

    /**
     * Get count of resource for an id.
     * @returns: Count of resource type.
     */
    uint32_t get(uint32_t id, ResourceType resource_type);

    /**
     * Get counts for all resources for an id.
     * @returns: Array of counts where index corresponds to ResourceType value.
     */
    std::array<uint32_t, RESOURCE_TYPE_COUNT> get(uint32_t id);

    /**
     * Set a count of resource for an id.
     */
    void set(uint32_t id, ResourceType resource_type, uint32_t count);

    /**
     * Add a count of resource to an id.
     * Note full count won't be added if id count reaches max.
     * @return: The count added and the resulting count.
     */
    std::pair<uint32_t, uint32_t> add(uint32_t id, ResourceType resource_type, uint32_t count);

    /**
     * Subtract a count of resource from an id.
     * Note full count will not be subtracted if id count reaches zero.
     * @return: The count subtracted and the resulting count.
     */
    std::pair<uint32_t, uint32_t> subtract(uint32_t id, ResourceType resource_type, uint32_t count);

    /**
     * Move a count of resource from one id to another.
     * Note full count won't be moved if sender count reaches zero or if reciever count reaches
     * max.
     * @return: The count subtracted, the count added, sender resulting count, reciever resulting
     * count.
     */
    std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>
    move(uint32_t from, uint32_t to, ResourceType resource_type, uint32_t count);

    /**
     * Get state update to send over the network.
     */
    ResourceControllerUpdate getUpdate();

    /**
     * Overwrite state with update.
     */
    void applyUpdate(ResourceControllerUpdate rcu);

  private:
    /**
     * Create id key with all resource counts set to 0.
     */
    void init(uint32_t id);

    std::unordered_map<uint32_t, std::array<uint32_t, RESOURCE_TYPE_COUNT>> m_resourceCounts;
};

#endif /* ResourceController_hpp */
