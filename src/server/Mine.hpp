#ifndef Mine_hpp
#define Mine_hpp

#include <stdio.h>
#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>
#include "../common/Circle.hpp"
#include "../common/CircleGameObject.hpp"

class Mine : public CircleGameObject {
 public:
     Mine(unsigned int id, sf::Vector2f position, float size);
     ~Mine();
     Mine(const Mine& temp_obj) = delete;  // TODO: define this
     Mine& operator=(const Mine& temp_obj) = delete;  // TODO: define this

     void update();
     static std::vector<std::shared_ptr<Mine>> getActiveMines(std::vector<std::shared_ptr<Mine>>& mines);
};

#endif /* Mine_hpp */
