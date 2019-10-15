#include <stdio.h>
#include <vector>
#include <memory>
#include <iostream>
#include <numeric>

#include "Mine.hpp"
#include "../util/game_settings.hpp"


sf::Packet& operator <<(sf::Packet& packet, const MineUpdate& mine_update) {
    return packet
        << mine_update.mine_id
        << mine_update.is_active
        << mine_update.x_pos
        << mine_update.y_pos
        << mine_update.radius;
}

sf::Packet& operator >>(sf::Packet& packet, MineUpdate& mine_update) {
    return packet
        >> mine_update.mine_id
        >> mine_update.is_active
        >> mine_update.x_pos
        >> mine_update.y_pos
        >> mine_update.radius;
}

Mine::Mine(uint32_t id, sf::Vector2f position, float size, sf::Color color,
  std::shared_ptr<PhysicsController> pc):
  CircleGameObject(id, position, size, color, pc) {}

Mine::~Mine() {}

MineUpdate Mine::getUpdate() {
    sf::Vector2f position = getCircle()->getPosition();
    MineUpdate mu = {(sf::Uint32) getId(), isActive(), position.x, position.y,
      getCircle()->getRadius()};
    return mu;
}

void Mine::applyUpdate(MineUpdate mu) {
    setActive(mu.is_active);
    setPosition(sf::Vector2f(mu.x_pos, mu.y_pos));
    setRadius(mu.radius);
}
