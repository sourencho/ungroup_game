#ifndef GroupShape_hpp
#define GroupShape_hpp

#include <SFML/Graphics.hpp>
#include <stdio.h>

class GroupShape {

    public:
        GroupShape(float size, sf::Vector2f position, sf::Color color);
        ~GroupShape();

        void draw(sf::RenderTarget& target);

        // Setters
        void move(const sf::Vector2f& offset);

    private:
        sf::CircleShape mCircle;
        sf::Vector2f mPosition;
        float mVelocity;
};

#endif /* GroupShape_hpp */
