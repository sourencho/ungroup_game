#ifndef ClientGameController_hpp
#define ClientGameController_hpp

#include <memory>
#include <queue>
#include <stdio.h>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../../common/bots/Bot.hpp"
#include "../../common/systems/GameController.hpp"
#include "../rendering/Animation.hpp"
#include "../rendering/AnimationController.hpp"
#include "../rendering/GUIController.hpp"
#include "../rendering/RenderingController.hpp"
#include "../systems/InputController.hpp"
#include "NetworkingClient.hpp"

class ClientGameController : public GameController {

  public:
    explicit ClientGameController(bool is_headless = false, bool is_bot = false,
                                  BotStrategy strategy = BotStrategy::Random,
                                  const std::string& server_ip = GAME_SETTINGS.LOCALHOST_IP,
                                  uint32_t server_tcp_port = GAME_SETTINGS.SERVER_TCP_PORT,
                                  LevelKey level_key = LevelKey::empty);
    ~ClientGameController();

    void start() override;

  private:
    // Overrides
    void update(const InputDef::PlayerInputs& pi, sf::Int32 delta_ms) override;
    void preUpdate() override;
    void postUpdate(sf::Int32 update_time) override;
    InputDef::PlayerInputs getPlayerInputs() override;
    void incrementTick() override;

    // Methods

    std::pair<InputDef::ReliableInput, InputDef::UnreliableInput> getBotMove(uint32_t bot_player_id,
                                                                             BotStrategy strategy);
    uint32_t getTick();
    void setTick(uint32_t tick);

    /**
     * Register the client via the NetworkingClient.
     * @returns: The client's player id.
     */
    void registerClient();

    void postRegisterInit();

    void draw();

    /*
     * Apply the game state update from the server (rewind, because it is likely the state of an old
     * tick) and then interpolate up to the current tick by applying local inputs for each tick
     * (replay).
     */
    void rewindAndReplay();

    void applyGameState(GameState& game_state);
    void replay(uint32_t client_tick, uint32_t server_tick);

    /**
     * Send client input to server
     */
    void sendInputs(std::pair<InputDef::ReliableInput, InputDef::UnreliableInput> inputs);

    /**
     * Save local input for replay
     */
    void saveInputs(std::pair<InputDef::ReliableInput, InputDef::UnreliableInput> inputs);

    // Variables
    uint32_t m_playerId = 0;
    bool m_headless, m_isBot;
    BotStrategy m_strategy;
    Bot m_bot;
    std::string m_serverIP;
    uint32_t m_largestAppliedGameStateTick = 0;

    std::unordered_map<uint32_t, InputDef::ClientInputAndTick>
        m_tickToInput; // Cache of past inputs

    sf::RenderWindow m_window;

    TemporalMetric m_networkUpdateMetric{
        120, sf::seconds(
                 0.5f)}; // Keeps track of the number of game state updates recieved from the server
    TemporalMetric m_tickDeltaMetric{
        5, sf::seconds(1)}; // Keeps track of the tick delta between the client and server
                            // based on updates recieved from the server

    std::unique_ptr<NetworkingClient> m_networkingClient;
    std::unique_ptr<InputController> m_inputController;
    std::unique_ptr<RenderingController> m_renderingController;
};

#endif /* ClientGameController_hpp */
