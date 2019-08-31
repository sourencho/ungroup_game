#ifndef ClientMine_hpp
#define ClientMine_hpp

#include <stdio.h>
#include <memory>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "../common/CircleGameObject.hpp"
#include "../common/game_def.hpp"
#include "../common/Mine.hpp"


class ClientMine : public CircleGameObject {
 public:
     explicit ClientMine(sf::Vector2f position, sf::Color color,
       std::shared_ptr<PhysicsController> pc);
     ~ClientMine();
     ClientMine(const ClientMine& temp_obj) = delete;  // TODO(sourenp): define this
     ClientMine& operator=(const ClientMine& temp_obj) = delete;  // TODO(sourenp): define this

     void applyUpdate(MineUpdate mu);
};

#endif /* ClientMine_hpp */
