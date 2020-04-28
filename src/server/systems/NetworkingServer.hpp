#ifndef NetworkingServer_hpp
#define NetworkingServer_hpp

#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <stdio.h>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include <SFML/Network.hpp>

#include "../../common/metrics/TemporalMetric.hpp"
#include "../../common/objects/Group.hpp"
#include "../../common/objects/Mine.hpp"
#include "../../common/util/InputDef.hpp"
#include "../../common/util/StateDef.hpp"

class NetworkingServer {
    const uint32_t CMD_DRIFT_THRESHOLD = 7;

  public:
    NetworkingServer();
    ~NetworkingServer();

    InputDef::PlayerInputs collectClientInputs();
    void setState(const GameState& gs);
    void setClientToPlayerId(int client_id, int player_id);
    void incrementTick();
    uint32_t getTick() const;
    void setTick(uint32_t tick);
    float getBroadcastGameStateRate();
    std::unordered_map<sf::Uint32, float> getPlayerUnreliableUpdatesRates();
    std::unordered_map<sf::Uint32, float> getPlayerTickDrifts();

  private:
    // Sockets
    sf::Uint16 createInputUdpSocket();
    sf::Uint16 createStateUdpSocket();

    std::mutex m_stateUdpSocket_lock;
    std::unique_ptr<sf::UdpSocket> m_stateUdpSocket_t;
    sf::Uint16 m_stateUdpSocketPort;

    std::unique_ptr<sf::UdpSocket> m_inputUdpSocket;
    sf::Uint16 m_inputUdpSocketPort;

    // Threads
    void reliableRecvSend();
    void unreliableRecv();
    void broadcastGameState();
    void natRecv();
    int32_t getPlayerIdFromClientId(uint32_t client_id);
    void updateDriftMetric(uint32_t player_id, uint32_t drift);
    void updateUpdatesMetric(uint32_t player_id);

    std::thread m_reliableRecvSend;
    std::thread m_unreliableRecv;
    std::thread m_broadcastGameStateThread;
    std::thread m_natRecvThread;

    std::atomic<bool> m_stopThreads_ta{false};

    // Methods
    void clientDisconnect(sf::TcpSocket& client, sf::Uint32 client_id);
    void registerClient(sf::Packet packet, sf::TcpSocket& socket, sf::Uint32 client_id);
    void sendPlayerId(sf::TcpSocket& socket, sf::Uint32 client_id);
    void handleReliableCommand(sf::Socket::Status status, sf::Packet command_packet,
                               sf::SocketSelector& selector, sf::TcpSocket& socket,
                               sf::Uint32 client_id);
    void handleUnreliableCommand(sf::Socket::Status status, sf::Packet command_packet,
                                 sf::IpAddress& sender, unsigned short port);
    void setClientReliableUpdate(sf::Packet packet, int client_id);
    void setClientUnreliableUpdate(sf::Packet packet, int client_id, uint32_t client_tick);
    std::vector<int> getClientIds();
    void sendGameState();
    void addEventListeners();
    void handlePlayerCreatedEvent(std::shared_ptr<Event> event);

    // Misc
    std::vector<std::pair<sf::Uint32, std::unique_ptr<sf::TcpSocket>>> m_clients;

    std::mutex m_clientToIps_lock;
    std::unordered_map<int, sf::IpAddress> m_clientToIps_t;

    std::mutex m_clientToPlayerIds_lock;
    std::unordered_map<int, sf::Uint32> m_clientToPlayerIds_t;

    std::mutex m_clientToUdpPorts_lock;
    std::unordered_map<int, sf::Uint16> m_clientToUdpPorts_t;

    std::mutex m_playerReliableUpdates_lock;
    std::vector<InputDef::PlayerReliableInput> m_playerReliableUpdates_t;

    std::mutex m_playerUnreliableUpdates_lock;
    std::vector<InputDef::PlayerUnreliableInput> m_playerUnreliableUpdates_t;

    std::mutex m_broadcastGameStateMetric_lock;
    TemporalMetric m_broadcastGameStateMetric_t{30, sf::seconds(0.5f)};

    std::mutex m_playerIdsToUpdatesMetric_lock;
    std::unordered_map<sf::Uint32, TemporalMetric*> m_playerIdsToUpdatesMetric_t;
    std::unordered_map<sf::Uint32, float> m_playerIdsToUpdatesRates_t;

    std::mutex m_playerIdsToDriftMetrics_lock;
    std::unordered_map<sf::Uint32, TemporalMetric*> m_playerIdsToDriftMetrics_t;
    std::unordered_map<sf::Uint32, float> m_playerIdsToDrifts_t;

    std::mutex m_gameState_lock;
    GameState m_gameState_t;

    sf::Uint32 m_clientIdCounter = 0;

    std::atomic<uint32_t> m_tick_ta{0};
};

#endif /* NetworkingServer_hpp */
