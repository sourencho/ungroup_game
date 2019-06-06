#ifndef GroupShape_hpp
#define GroupShape_hpp

#include <SFML/Graphics.hpp>
#include "../common/GameObject.hpp"
#include <stdio.h>

class GroupShape: public GameObject {

    public:
        GroupShape(float size, sf::Vector2f position);
        ~GroupShape();

        void draw(sf::RenderTarget& target);
        void move();
        void move(sf::Vector2f offset);

        // Getters
        sf::Vector2f getPosition() const;
        float getRadius() const;
        sf::Vector2f getVelocity() const;

        // Setters
        void setPosition(sf::Vector2f position);
        void setRadius(int size);
        void setVelocity(sf::Vector2f velocity);

    private:
        sf::CircleShape mCircle;
        sf::Vector2f mVelocity;

};

#endif /* GroupShape_hpp */
