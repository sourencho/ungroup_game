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

    private:
        sf::CircleShape mCircleShape;

};

#endif /* Circle_hpp */
