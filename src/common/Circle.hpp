#ifndef Circle_hpp
#define Circle_hpp

#include <SFML/Graphics.hpp>
#include <memory>
#include <stdio.h>

class Circle {

    public:
        Circle(float size, sf::Vector2f position, sf::Color color);
        ~Circle();

        void draw(sf::RenderTarget& target, sf::Shader* shader, bool use_shader);
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
        void setColor();
        void setColor(sf::Color color);
        void changeColor(sf::Color color);

    private:
        sf::CircleShape mCircleShape;
        sf::Vector2f mVelocity;
        sf::Color mColor;

};
#endif /* Circle_hpp */
