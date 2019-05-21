#ifndef Client_hpp
#define Client_hpp

#include <SFML/Graphics.hpp>
#include "Group.hpp"
#include "GroupShape.hpp"
#include <stdio.h>

class Client {

    public:
        Client(int max_player_count, sf::Keyboard::Key keys[4]);
        ~Client();

        void draw(sf::RenderTarget& target);
        void update(std::vector<Group*> groups);
        void handleEvents(sf::Event& event);

        // Setters
        void setId(int id);

        // Getters
        int getId() const;
        sf::Vector2f getDirection() const;

    private:
        std::vector<GroupShape*> mGroupShapes;
        sf::Vector2f mDirection;
        sf::Keyboard::Key mUp;
        sf::Keyboard::Key mDown;
        sf::Keyboard::Key mLeft;
        sf::Keyboard::Key mRigth;
        int mId;

};

#endif /* Client_hpp */
