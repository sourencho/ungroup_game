#ifndef Group_h
#define Group_h

#include <SFML/Graphics.hpp>

class Group{

    public:
        Group(float size, sf::Color color);
        ~Group();
    
        void update();
        void draw(sf::RenderTarget& target);
        void handleEvents(sf::Event& event);
    
    private:
        sf::CircleShape mCircle;
        sf::Vector2f mPosition;
};

#endif /* Group_h */
