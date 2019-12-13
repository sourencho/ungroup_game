#ifndef GroupController_hpp
#define GroupController_hpp

#include <memory>
#include <unordered_map>
#include <vector>

#include "../events/Event.hpp"
#include "../objects/Group.hpp"
#include "../objects/Player.hpp"
#include "../resources/ResourceStore.hpp"
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
                    std::vector<std::shared_ptr<Player>>& players, ResourceStore& rs);
    ~GroupController(){};
    GroupController(const GroupController& temp_obj) = delete;
    GroupController& operator=(const GroupController& temp_obj) = delete;

    /**
     * Draw the game object's onto the scalable buffer;
     */
    void draw(sf::RenderTarget& buffer);

    /**
     * Draw the game object specific UI onto the unscaled window.
     * This is called after draw, so the UI is drawn over the game objects.
     */
    void drawUI(sf::RenderWindow& window, sf::View& player_view);

    void update();
    uint32_t createGroup(uint32_t player_id);
    void updatePostPhysics();
    GroupControllerUpdate getUpdate();
    void applyUpdate(GroupControllerUpdate gcu);
    uint32_t getGroupId(uint32_t player_id);
    Group& getGroup(uint32_t group_id);
    std::vector<uint32_t> getGroupPlayerIds(uint32_t group_id);

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
    std::vector<sf::Vector2f> getPlayerDirections(uint32_t group_id);
    std::vector<ResourceType> getPlayerIntents(uint32_t group_id);

    /**
     * Group is ungroup if any member player is ungroup
     */
    bool getUngroup(uint32_t group_id);

    /**
     * Group is joinable if any member player is joinable
     */
    bool getJoinable(uint32_t group_id);

    /**
     * Draws groups' player ids as text at the cneter of each group.
     */
    void drawGroupPlayerIds(sf::RenderWindow& window, sf::View& player_view);

    std::vector<std::shared_ptr<Player>> m_players;
    std::vector<std::shared_ptr<Group>> m_groups;
    std::vector<sf::Text> m_groupPlayerTexts;
    std::unordered_map<uint32_t, std::vector<uint32_t>> m_groupToPlayers;
    std::unordered_map<uint32_t, uint32_t> m_playerToGroup;
    ResourceStore& m_resourceStore;

    size_t nextGroupIndex = 0;
};

#endif /* GroupController_hpp */
