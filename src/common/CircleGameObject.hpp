#ifndef CircleGameObject_hpp
#define CircleGameObject_hpp

#include <vector>
#include <memory>

#include "GameObject.hpp"
#include "../common/Circle.hpp"

class CircleGameObject : public GameObject {
    public:
        CircleGameObject(unsigned int id, sf::Vector2f position, float size, sf::Color color);
        std::shared_ptr<Circle> getCircle();
        static std::vector<std::shared_ptr<Circle>> getCircles(
            std::vector<std::shared_ptr<CircleGameObject>>& circle_game_objects);
    protected:
        std::shared_ptr<Circle> mCircle;
};

#endif /* CircleGameObject_hpp */
