#ifndef Circle_hpp
#define Circle_hpp

#include <SFML/Graphics.hpp>
#include <memory>
#include <stdio.h>

class Circle {

    public:
        Circle(float size, sf::Vector2f position);
        ~Circle();

        void draw(sf::RenderTarget& target, sf::Shader* shader, bool use_shader);

        sf::Vector2f getPosition() const;
        void setPosition(sf::Vector2f position);

        float getRadius() const;
        void setRadius(int size);

        // TODO(souren): On client use rigid body instead of these to move circles
        void move();
        void move(sf::Vector2f offset);
        sf::Vector2f getVelocity() const;
        void setVelocity(sf::Vector2f velocity);

    private:
        sf::CircleShape mCircleShape;
        sf::Vector2f mVelocity;

};

#endif /* Circle_hpp */
