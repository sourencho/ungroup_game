#ifndef ClientMine_hpp
#define ClientMine_hpp

#include <stdio.h>
#include <memory>
#include <iostream>

#include <SFML/Graphics.hpp>
#include "../common/CircleGameObject.hpp"

class ClientMine : public CircleGameObject {
 public:
     explicit ClientMine(sf::Vector2f position, std::shared_ptr<CircleRigidBody> crb);
     ~ClientMine();
     ClientMine(const ClientMine& temp_obj) = delete;  // TODO(sourenp): define this
     ClientMine& operator=(const ClientMine& temp_obj) = delete;  // TODO(sourenp): define this
};

#endif /* ClientMine_hpp */
