#ifndef ClientMine_hpp
#define ClientMine_hpp

#include <stdio.h>

#include <SFML/Graphics.hpp>
#include "../common/Circle.hpp"
#include "../common/GameObject.hpp"

class ClientMine : public GameObject {
 public:
     explicit ClientMine(sf::Vector2f position);
     ~ClientMine();
     ClientMine(const ClientMine& temp_obj) = delete;  // TODO(sourenp): define this
     ClientMine& operator=(const ClientMine& temp_obj) = delete;  // TODO(sourenp): define this

     Circle* getCircle();

 private:
     Circle* mCircle;
};

#endif /* ClientMine_hpp */
