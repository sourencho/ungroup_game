#include "ClientMine.hpp"

ClientMine::ClientMine(sf::Vector2f position, sf::Color color,
    std::shared_ptr<PhysicsController> pc)
    :CircleGameObject(0, position, 0.f, color, pc) {}
ClientMine::~ClientMine() {}
