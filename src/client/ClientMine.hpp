#ifndef ClientMine_hpp
#define ClientMine_hpp

#include <stdio.h>

#include <SFML/Graphics.hpp>
#include "../common/CircleGameObject.hpp"

class ClientMine : public CircleGameObject {
 public:
     explicit ClientMine(sf::Vector2f position);
     ~ClientMine();
     ClientMine(const ClientMine& temp_obj) = delete;  // TODO(sourenp): define this
     ClientMine& operator=(const ClientMine& temp_obj) = delete;  // TODO(sourenp): define this
};

#endif /* ClientMine_hpp */
