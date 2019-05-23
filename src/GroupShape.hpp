#ifndef GroupShape_hpp
#define GroupShape_hpp

#include <SFML/Graphics.hpp>
#include "GameObject.hpp"
#include <stdio.h>

class GroupShape: public GameObject {

    public:
        GroupShape(float size, sf::Vector2f position, sf::Color color);
        ~GroupShape();

        void draw(sf::RenderTarget& target);

        // Getters
        sf::Vector2f getPosition() const;
        float getRadius() const;

        // Setters
        void setPosition(sf::Vector2f position);
        void setRadius(int size);

    private:
        sf::CircleShape mCircle;
};

#endif /* GroupShape_hpp */
