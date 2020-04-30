#ifndef game_settings_hpp
#define game_settings_hpp

#include <chrono>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "../systems/ResourceController.hpp"
#include "InputDef.hpp"

const InputDef::InputKeys INPUT_KEYS = {
    .up = sf::Keyboard::Up,
    .down = sf::Keyboard::Down,
    .right = sf::Keyboard::Right,
    .left = sf::Keyboard::Left,
    .joinable = sf::Keyboard::G,
    .ungroup = sf::Keyboard::Escape,
    .stop = sf::Keyboard::Space,
    .intent = sf::Keyboard::E,
};

struct GameSettings {
    int MAX_PLAYER_COUNT;
    int MAX_MINE_COUNT;
    float MINE_SIZE;
    float GROUP_MEMBER_SIZE;
    float GROUP_SPEED;
    uint32_t MINE_RESOURCE_COUNT;
    float GAME_BOUNDS_RADIUS;
    sf::Vector2f GAME_CENTER;
    sf::Vector2f GAME_SIZE;
    uint32_t TOTAL_RESOURCE_REQUIREMENTS;
    float MIN_TIME_STEP_SEC;
    std::chrono::milliseconds CLIENT_FETCH_PLAYER_ID_SLEEP;
    std::chrono::milliseconds SERVER_INPUT_WINDOW_SLEEP;
    std::chrono::milliseconds CLIENT_RELIABLE_SEND_SLEEP;
    std::chrono::milliseconds CLIENT_UNRELIABLE_SEND_SLEEP;
    std::chrono::milliseconds SERVER_RELIABLE_REVC_SEND_SLEEP;
    std::chrono::milliseconds CLIENT_RELIBALE_RECV_SLEEP;
    std::chrono::milliseconds SERVER_BROADCAST_GAME_STATE_SLEEP;
    std::chrono::milliseconds CLIENT_UNRELIABLE_RECV_SLEEP;
    std::chrono::milliseconds CLIENT_NAT_SEND_SLEEP;
    std::chrono::milliseconds SERVER_NAT_RECV_SLEEP;
    sf::Time SERVER_UNRELIABLE_RECV_TIMEOUT_SEC;
    unsigned short SERVER_TCP_PORT;
    unsigned short SERVER_STATE_UDP_PORT;
    unsigned short SERVER_INPUT_UDP_PORT;
    std::string LOCALHOST_IP;
    bool REPLAY;
    int REPLAY_TICK_DELTA_THRESHOLD;
};

extern const GameSettings GAME_SETTINGS;

#endif /* game_settings_hpp */
