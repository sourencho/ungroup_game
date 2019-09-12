#ifndef ClientMine_hpp
#define ClientMine_hpp

#include <stdio.h>
#include <memory>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "../../common/objects/CircleGameObject.hpp"
#include "../../common/util/game_def.hpp"
#include "../../common/objects/Mine.hpp"


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
