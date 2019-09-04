#ifndef Mine_hpp
#define Mine_hpp

#include <stdio.h>
#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include "../common/Circle.hpp"
#include "../common/CircleGameObject.hpp"


struct MineUpdate {
    sf::Uint32 mine_id;
    bool is_active;
    float x_pos;
    float y_pos;
    float radius;
};

sf::Packet& operator <<(sf::Packet& packet, const MineUpdate& mine_update);
sf::Packet& operator >>(sf::Packet& packet, MineUpdate& mine_update);

class Mine : public CircleGameObject {
 public:
     Mine(unsigned int id, sf::Vector2f position, float size, sf::Color color,
        std::shared_ptr<PhysicsController> pc);
     ~Mine();
     Mine(const Mine& temp_obj) = delete;  // TODO: define this
     Mine& operator=(const Mine& temp_obj) = delete;  // TODO: define this

     MineUpdate getUpdate();
};

#endif /* Mine_hpp */
