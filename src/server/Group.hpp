#ifndef Group_hpp
#define Group_hpp

#include <stdio.h>
#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "../common/Circle.hpp"

class Group : public GameObject {
 public:
     Group(int id, sf::Vector2f position);
     ~Group();
     Group(const Group& temp_obj) = delete;  // TODO: define this
     Group& operator=(const Group& temp_obj) = delete;  // TODO: define this

     void update();
     void addMember(std::shared_ptr<Player> player);
     static void handleCollisions(std::vector<std::shared_ptr<Group>>& groups);

     // Getters
     int getId() const;
     std::shared_ptr<Circle> getCircle();

 private:
     int mId;
     float mSize;
     std::vector<std::shared_ptr<Player>> mMembers;
     void refresh();
     std::shared_ptr<Circle> mCircle;
};

#endif /* Group_hpp */
