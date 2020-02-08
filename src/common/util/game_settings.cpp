#include "game_settings.hpp"

#include <fstream>
#include <iostream>

#include "../../../extern/nlohmann/json.hpp"

using json = nlohmann::json;

GameSettings load_game_setting_globals(const std::string& path) {
    std::ifstream i(path);
    json j;
    i >> j;

    int MAX_PLAYER_COUNT = j["gameplay"]["MAX_PLAYER_COUNT"].get<int>();
    int MAX_MINE_COUNT = j["gameplay"]["MAX_MINE_COUNT"].get<int>();
    float GROUP_MEMBER_SIZE = j["gameplay"]["GROUP_MEMBER_SIZE"].get<float>();
    float MINE_SIZE = j["gameplay"]["MINE_SIZE"].get<float>();
    float GROUP_SPEED = j["gameplay"]["GROUP_SPEED"].get<float>();
    uint32_t MINE_RESOURCE_COUNT = j["gameplay"]["MINE_RESOURCE_COUNT"].get<uint32_t>();
    float GAME_BOUNDS_RADIUS = j["gameplay"]["GAME_BOUNDS_RADIUS"].get<float>();
    sf::Vector2f GAME_CENTER(GAME_BOUNDS_RADIUS, GAME_BOUNDS_RADIUS);
    sf::Vector2f GAME_SIZE(GAME_BOUNDS_RADIUS * 2.f, GAME_BOUNDS_RADIUS * 2.f);

    std::chrono::milliseconds CLIENT_FETCH_PLAYER_ID_SLEEP(
        j["threads"]["CLIENT_FETCH_PLAYER_ID_SLEEP"].get<uint32_t>());
    std::chrono::milliseconds SERVER_INPUT_WINDOW_SLEEP(
        j["threads"]["SERVER_INPUT_WINDOW_SLEEP"].get<uint32_t>());
    std::chrono::milliseconds CLIENT_RELIABLE_SEND_SLEEP(
        j["threads"]["CLIENT_RELIABLE_SEND_SLEEP"].get<uint32_t>());
    std::chrono::milliseconds CLIENT_UNRELIABLE_SEND_SLEEP(
        j["threads"]["CLIENT_UNRELIABLE_SEND_SLEEP"].get<uint32_t>());
    std::chrono::milliseconds SERVER_RELIABLE_REVC_SEND_SLEEP(
        ((CLIENT_RELIABLE_SEND_SLEEP / MAX_PLAYER_COUNT) / 2));
    std::chrono::milliseconds CLIENT_RELIBALE_RECV_SLEEP(SERVER_RELIABLE_REVC_SEND_SLEEP / 2);
    std::chrono::milliseconds SERVER_BROADCAST_GAME_STATE_SLEEP(
        j["threads"]["SERVER_BROADCAST_GAME_STATE_SLEEP"].get<uint32_t>());
    std::chrono::milliseconds CLIENT_UNRELIABLE_RECV_SLEEP(SERVER_BROADCAST_GAME_STATE_SLEEP / 2);
    std::chrono::milliseconds CLIENT_NAT_SEND_SLEEP(
        j["threads"]["CLIENT_NAT_SEND_SLEEP"].get<uint32_t>());
    std::chrono::milliseconds SERVER_NAT_RECV_SLEEP(CLIENT_NAT_SEND_SLEEP / 2);

    sf::Time SERVER_UNRELIABLE_RECV_TIMEOUT_SEC(
        sf::seconds(j["networking"]["SERVER_UNRELIABLE_RECV_TIMEOUT_SEC"].get<uint64_t>()));

    unsigned short SERVER_TCP_PORT = j["networking"]["SERVER_TCP_PORT"].get<unsigned short>();
    unsigned short SERVER_STATE_UDP_PORT =
        j["networking"]["SERVER_STATE_UDP_PORT"].get<unsigned short>();
    unsigned short SERVER_INPUT_UDP_PORT =
        j["networking"]["SERVER_INPUT_UDP_PORT"].get<unsigned short>();
    std::string LOCALHOST_IP = j["networking"]["LOCALHOST_IP"].get<std::string>();

    GameSettings game_settings = {
        .MAX_PLAYER_COUNT = MAX_PLAYER_COUNT,
        .MAX_MINE_COUNT = MAX_MINE_COUNT,
        .MINE_SIZE = MINE_SIZE,
        .GROUP_MEMBER_SIZE = GROUP_MEMBER_SIZE,
        .GROUP_SPEED = GROUP_SPEED,
        .MINE_RESOURCE_COUNT = MINE_RESOURCE_COUNT,
        .GAME_BOUNDS_RADIUS = GAME_BOUNDS_RADIUS,
        .GAME_CENTER = GAME_CENTER,
        .GAME_SIZE = GAME_SIZE,
        .CLIENT_FETCH_PLAYER_ID_SLEEP = CLIENT_FETCH_PLAYER_ID_SLEEP,
        .SERVER_INPUT_WINDOW_SLEEP = SERVER_INPUT_WINDOW_SLEEP,
        .CLIENT_RELIABLE_SEND_SLEEP = CLIENT_RELIABLE_SEND_SLEEP,
        .CLIENT_UNRELIABLE_SEND_SLEEP = CLIENT_UNRELIABLE_SEND_SLEEP,
        .SERVER_RELIABLE_REVC_SEND_SLEEP = SERVER_RELIABLE_REVC_SEND_SLEEP,
        .CLIENT_RELIBALE_RECV_SLEEP = CLIENT_RELIBALE_RECV_SLEEP,
        .SERVER_BROADCAST_GAME_STATE_SLEEP = SERVER_BROADCAST_GAME_STATE_SLEEP,
        .CLIENT_UNRELIABLE_RECV_SLEEP = CLIENT_UNRELIABLE_RECV_SLEEP,
        .CLIENT_NAT_SEND_SLEEP = CLIENT_NAT_SEND_SLEEP,
        .SERVER_NAT_RECV_SLEEP = SERVER_NAT_RECV_SLEEP,
        .SERVER_UNRELIABLE_RECV_TIMEOUT_SEC = SERVER_UNRELIABLE_RECV_TIMEOUT_SEC,
        .SERVER_TCP_PORT = SERVER_TCP_PORT,
        .SERVER_STATE_UDP_PORT = SERVER_STATE_UDP_PORT,
        .SERVER_INPUT_UDP_PORT = SERVER_INPUT_UDP_PORT,
        .LOCALHOST_IP = LOCALHOST_IP,
    };
    return game_settings;
};

const GameSettings GAME_SETTINGS = load_game_setting_globals("data/configs/game_settings.json");
