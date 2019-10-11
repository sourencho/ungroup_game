#ifndef GroupController_hpp
#define GroupController_hpp

#include <vector>
#include <memory>
#include <unordered_map>

#include "../../common/objects/Player.hpp"
#include "../../common/objects/Group.hpp"

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

sf::Packet& operator <<(sf::Packet& packet, const GroupIdAndPlayerIds& gipi);
sf::Packet& operator >>(sf::Packet& packet, GroupIdAndPlayerIds& gipi);

sf::Packet& operator <<(sf::Packet& packet, const GroupControllerUpdate& gcu);
sf::Packet& operator >>(sf::Packet& packet, GroupControllerUpdate& gcu);

/* Class */

class GroupController {
 public:
    GroupController(std::vector<std::shared_ptr<Group>>& groups,
        std::vector<std::shared_ptr<Player>>& players);
    ~GroupController();
    GroupController(const GroupController& temp_obj) = delete;
    GroupController& operator=(const GroupController& temp_obj) = delete;

    void update();
    int createGroup(int player_id);
    void updatePostPhysics();
    GroupControllerUpdate getUpdate();
    void applyUpdate(GroupControllerUpdate gcu);

 private:
    void refreshGroup(std::shared_ptr<Group>& group);
    void updateGroup(std::shared_ptr<Group>& group);

    std::vector<std::shared_ptr<Player>> mPlayers;
    std::vector<std::shared_ptr<Group>> mGroups;
    std::unordered_map<sf::Uint32, std::vector<sf::Uint32>> mGroupToPlayers;
};


#endif /* GroupController_hpp */
