#ifndef Mine_hpp
#define Mine_hpp

#include <stdio.h>
#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>
#include "../common/Circle.hpp"
#include "../common/GameObject.hpp"

class Mine : public GameObject {
 public:
     Mine(int id, sf::Vector2f position, float size);
     ~Mine();
     Mine(const Mine& temp_obj) = delete;  // TODO: define this
     Mine& operator=(const Mine& temp_obj) = delete;  // TODO: define this

     void update();
     static std::vector<std::shared_ptr<Mine>> getActiveMines(std::vector<std::shared_ptr<Mine>>& mines);
     static std::vector<std::shared_ptr<Circle>> getCircles(std::vector<std::shared_ptr<Mine>>& mines);

     // Getters
     int getId() const;
     std::shared_ptr<Circle> getCircle();

 private:
     int mId;
     void refresh();
     std::shared_ptr<Circle> mCircle;
};

#endif /* Mine_hpp */
