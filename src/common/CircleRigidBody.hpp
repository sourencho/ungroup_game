#ifndef CircleRigidBody_hpp
#define CircleRigidBody_hpp

#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>
#include "GameObject.hpp"

class CircleRigidBody {
 public:
    CircleRigidBody(float radius, sf::Vector2f position);

    void setActive(bool is_active);
    bool isActive() const;

    float getRadius() const;
    void setRadius(float radius);

    sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f position);

    void setVelocity(sf::Vector2f velocity);

    void move(sf::Vector2f offset);
    void step();

 private:
    bool mIsActive = false;
    float mRadius;
    sf::Vector2f mPosition;
    sf::Vector2f mVelocity;
};

#endif /* CircleRigidBody_hpp */
