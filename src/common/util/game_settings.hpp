#ifndef game_settings_hpp
#define game_settings_hpp

#include <chrono>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "../systems/ResourceController.hpp"
#include "InputDef.hpp"

/* Input */

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

/* Rendering */
const float GAME_BOUNDS_RADIUS = 500;
const sf::Vector2f WINDOW_RESOLUTION(1200, 1200);
const sf::Vector2f GAME_CENTER(GAME_BOUNDS_RADIUS, GAME_BOUNDS_RADIUS);
const sf::Vector2f GAME_SIZE(GAME_BOUNDS_RADIUS * 2.f, GAME_BOUNDS_RADIUS * 2.f);
const float GAME_SCALE = 5.f;
const sf::Vector2f GAME_SCALING_FACTOR(GAME_SCALE* GAME_SIZE.x / WINDOW_RESOLUTION.x,
                                       GAME_SCALE* GAME_SIZE.y / WINDOW_RESOLUTION.y);

const bool SHOW_DIRECTION_ARROWS = false;
const bool SHOW_DIRECTION_LINES = true;
const bool SHOW_PLAYER_IDS = true;

/* Game Logic */
const bool USE_INTERPOLATION_REPLAY = false;

const int NAT_PUNCH_INTERVAL = 30;
const int MAX_PLAYER_COUNT = 20;
const int MAX_MINE_COUNT = 8;
const float GROUP_MEMBER_SIZE = 10.f;
const float MINE_SIZE = 40.f;
const float GROUP_SPEED = 100.f;
const uint32_t MINE_RESOURCE_COUNT = 10;

const std::array<uint32_t, RESOURCE_TYPE_COUNT> WIN_CONDITION = {
    MINE_RESOURCE_COUNT,
    MINE_RESOURCE_COUNT,
    MINE_RESOURCE_COUNT,
    MINE_RESOURCE_COUNT,
};

/* Threads */

const std::chrono::milliseconds CLIENT_FETCH_PLAYER_ID_SLEEP(16);

const std::chrono::milliseconds SERVER_INPUT_WINDOW_SLEEP(50);

const std::chrono::milliseconds CLIENT_RELIABLE_SEND_SLEEP(16);
const std::chrono::milliseconds
    SERVER_RELIABLE_REVC_SEND_SLEEP((CLIENT_RELIABLE_SEND_SLEEP / MAX_PLAYER_COUNT) / 2);
const std::chrono::milliseconds CLIENT_RELIBALE_RECV_SLEEP(SERVER_RELIABLE_REVC_SEND_SLEEP / 2);

const std::chrono::milliseconds CLIENT_UNRELIABLE_SEND_SLEEP(16);

const std::chrono::milliseconds SERVER_BROADCAST_GAME_STATE_SLEEP(16);
const std::chrono::milliseconds CLIENT_UNRELIABLE_RECV_SLEEP(SERVER_BROADCAST_GAME_STATE_SLEEP / 2);

const std::chrono::milliseconds CLIENT_NAT_SEND_SLEEP(3000);
const std::chrono::milliseconds SERVER_NAT_RECV_SLEEP(CLIENT_NAT_SEND_SLEEP / MAX_PLAYER_COUNT);

const sf::Time SERVER_UNRELIABLE_RECV_TIMEOUT = sf::seconds(1);

/* Networking */
static const unsigned short SERVER_TCP_PORT = 4844;
static const unsigned short SERVER_STATE_UDP_PORT = 4845;
static const unsigned short SERVER_INPUT_UDP_PORT = 4846;
static const std::string LOCALHOST_IP = "127.0.0.1";

#endif /* game_settings_hpp */
