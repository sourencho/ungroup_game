#include "ClientMine.hpp"


ClientMine::ClientMine(sf::Vector2f position, sf::Color color,
  std::shared_ptr<PhysicsController> pc):
  CircleGameObject(0, position, 0.f, color, pc) {}

ClientMine::~ClientMine() {}

void ClientMine::applyUpdate(MineUpdate mu) {
    setActive(mu.is_active);
    setPosition(sf::Vector2f(mu.x_pos, mu.y_pos));
    setRadius(mu.radius);
}
