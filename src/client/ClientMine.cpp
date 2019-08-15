#include "ClientMine.hpp"

ClientMine::ClientMine(sf::Vector2f position, std::shared_ptr<PhysicsController> pc)
    :CircleGameObject(0, position, 0.f, pc) {}
ClientMine::~ClientMine() {}
