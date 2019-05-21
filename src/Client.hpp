#ifndef Client_hpp
#define Client_hpp

#include <SFML/Graphics.hpp>
#include "Group.hpp"
#include "GroupShape.hpp"
#include "game_def.hpp"
#include <stdio.h>

class Client {

    public:
        Client(int max_player_count, sf::Keyboard::Key keys[4]);
        ~Client();

        void draw(sf::RenderTarget& target);
        void update(std::vector<Group*> active_groups);
        void handleEvents(sf::Event& event);

        // Setters
        void setId(int id);

        // Getters
        int getId() const;
        sf::Vector2f getDirection() const;

    private:
        std::vector<GroupShape*> mGroupShapes;
        sf::Vector2f mDirection;
        keys mKeys;
        int mId;

};

#endif /* Client_hpp */
