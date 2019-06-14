#ifndef Circle_hpp
#define Circle_hpp

#include <SFML/Graphics.hpp>
#include <stdio.h>

class Circle {

    public:
        Circle(float size, sf::Vector2f position);
        ~Circle();

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
        sf::CircleShape mCircleShape;
        sf::Vector2f mVelocity;

};

#endif /* Circle_hpp */
