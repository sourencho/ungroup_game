#ifndef GroupShape_hpp
#define GroupShape_hpp

#include <SFML/Graphics.hpp>
#include <stdio.h>

class GroupShape {

    public:
        GroupShape(float size, sf::Vector2f position, sf::Color color);
        ~GroupShape();

        void draw(sf::RenderTarget& target);

        // Getters
        sf::Vector2f getPosition() const;
        float getRadius() const;
        bool isActive() const;

        // Setters
        void setPosition(sf::Vector2f position);
        void setRadius(int size);
        void setActive(bool is_active);

    private:
        sf::CircleShape mCircle;
        bool mIsActive;
};

#endif /* GroupShape_hpp */
