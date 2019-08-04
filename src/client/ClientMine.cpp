#include "ClientMine.hpp"

ClientMine::ClientMine(sf::Vector2f position, std::shared_ptr<CircleRigidBody> crb)
    :CircleGameObject(0, position, 0.f, crb) {}
ClientMine::~ClientMine() {}
