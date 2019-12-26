#ifndef GroupController_hpp
#define GroupController_hpp

#include <memory>
#include <unordered_map>
#include <vector>

#include "../events/Event.hpp"
#include "../objects/Group.hpp"
#include "../objects/Player.hpp"
#include "../util/TypeDef.hpp"

/* Network utilities */

struct GroupIdAndPlayerIds {
    sf::Uint32 group_id;
    sf::Uint32 player_ids_size;
    std::vector<sf::Uint32> player_ids;
};

struct GroupControllerUpdate {
    sf::Uint32 group_id_and_player_ids_size;
    std::vector<GroupIdAndPlayerIds> group_id_and_player_idss;
};

sf::Packet& operator<<(sf::Packet& packet, const GroupIdAndPlayerIds& gipi);
sf::Packet& operator>>(sf::Packet& packet, GroupIdAndPlayerIds& gipi);

sf::Packet& operator<<(sf::Packet& packet, const GroupControllerUpdate& gcu);
sf::Packet& operator>>(sf::Packet& packet, GroupControllerUpdate& gcu);

/* Class */

class GroupController {
  public:
    GroupController(std::vector<std::shared_ptr<Group>>& groups,
                    std::vector<std::shared_ptr<Player>>& players, ResourceController& rc);
    ~GroupController(){};
    GroupController(const GroupController& temp_obj) = delete;
    GroupController& operator=(const GroupController& temp_obj) = delete;

    void update();
    uint32_t createGroup(uint32_t player_id);
    void updatePostPhysics();
    GroupControllerUpdate getUpdate();
    void applyUpdate(GroupControllerUpdate gcu);
    uint32_t getGroupId(uint32_t player_id);
    Group& getGroup(uint32_t group_id);
    std::vector<uint32_t> getGroupPlayerIds(uint32_t group_id);

    std::vector<uint32_t> getGroupIds();
    std::vector<sf::Vector2f> getPlayerDirections(uint32_t group_id);
    std::vector<ResourceType> getPlayerIntents(uint32_t group_id);
    std::array<uint32_t, RESOURCE_TYPE_COUNT> getResources(uint32_t group_id);

    /**
     * Group is ungroup if any member player is ungroup
     */
    bool getUngroup(uint32_t group_id);

    /**
     * Group is joinable if any member player is joinable
     */
    bool getJoinable(uint32_t group_id);

  private:
    std::vector<uint32_t> getEmptyGroupIds();
    std::pair<TypeDef::ids, TypeDef::ids> partitionGroupsByPlayerCount();
    Player& getPlayer(uint32_t player_id);
    void handleCollisionEvent(std::shared_ptr<Event> event);
    void handleClientDisconnectedEvent(std::shared_ptr<Event> event);
    void joinGroups(uint32_t circle_a_id, uint32_t circle_b_id);
    void regroup(std::vector<std::shared_ptr<Group>>& groups);
    void refreshGroup(std::shared_ptr<Group>& group);
    void updateGroup(std::shared_ptr<Group>& group);
    void removePlayer(uint32_t player_id);
    void addEventListeners();

    std::vector<std::shared_ptr<Player>> m_players;
    std::vector<std::shared_ptr<Group>> m_groups;
    std::unordered_map<uint32_t, std::vector<uint32_t>> m_groupToPlayers;
    std::unordered_map<uint32_t, uint32_t> m_playerToGroup;
    ResourceController& m_resourceController;

    size_t nextGroupIndex = 0;
};

#endif /* GroupController_hpp */
