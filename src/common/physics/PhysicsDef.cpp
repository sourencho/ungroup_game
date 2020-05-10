#include "PhysicsDef.hpp"

sf::Packet& operator<<(sf::Packet& packet, const Collision& collision) {
    packet << static_cast<sf::Uint32>(collision.ids.first)
           << static_cast<sf::Uint32>(collision.ids.second) << collision.position.x
           << collision.position.y << collision.normal.x << collision.normal.y
           << collision.resolution.first.x << collision.resolution.first.y
           << collision.resolution.second.x << collision.resolution.second.y << collision.collided;
    return packet;
};

sf::Packet& operator>>(sf::Packet& packet, Collision& collision) {
    sf::Uint32 id_first, id_second;
    float pos_x, pos_y, nor_x, nor_y, res_first_x, res_second_x, res_first_y, res_second_y;
    bool collided;
    packet >> id_first >> id_second >> pos_x >> pos_y >> nor_x >> nor_y >> res_first_x >>
        res_first_y >> res_second_x >> res_second_y >> collided;
    collision = {
        std::make_pair(static_cast<uint32_t>(id_first), static_cast<uint32_t>(id_second)),
        sf::Vector2f(pos_x, pos_y),
        sf::Vector2f(nor_x, nor_y),
        std::make_pair(sf::Vector2f(res_first_x, res_first_y),
                       sf::Vector2f(res_second_x, res_second_y)),
        collided,
    };
    return packet;
};