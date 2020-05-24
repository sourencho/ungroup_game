#ifndef ClientGameController_hpp
#define ClientGameController_hpp

#include <memory>
#include <queue>
#include <random>
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

    /**
     * Update game state by stalling, interpolating and/or extrapolating.
     */
    void updateGameState(const InputDef::PlayerInputs& pi, sf::Int32 delta_ms);

    /**
     * Interpolate game state from start_tick to to_tick using end_tick's game state.
     * For example, if start tick is 10, end tick is 20 and to_tick is 11 then we will interpolate
     * 10% from the local game state to the game state passed in.
     */
    void interpolateGameState(uint32_t start_tick, uint32_t to_tick, uint32_t end_tick,
                              const GameState& game_state);

    /**
     * Locally apply game state events recieved from the server.
     */
    void applyGameStateEvents(const GameStateEvent& game_state_event);

    /**
     * Handle local state is behind smallest server game state in the buffer.
     */
    void behind(uint32_t client_tick, uint32_t smallest_server_tick,
                const GameState& smallest_server_game_state);

    /**
     * Handle local state is ahead of smallest server game state buffer.
     */
    void ahead(uint32_t client_tick, uint32_t smallest_server_tick,
               std::map<uint32_t, GameState>& game_state_buffer);

    /**
     * Send client input to server
     */
    void sendInputs(std::pair<InputDef::ReliableInput, InputDef::UnreliableInput> inputs);

    // Variables
    uint32_t m_playerId = 0;
    bool m_headless, m_isBot;
    BotStrategy m_strategy;
    Bot m_bot;
    std::string m_serverIP;

    sf::RenderWindow m_window;

    TemporalMetric m_tickDeltaMetric{
        4 * 15, sf::seconds(0.25f)}; // Tracks the absolute tick delta between the client and target
                                     // server state. Each count is a tick delta.
    TemporalMetric m_behindGameStateMetric{
        4 * 15, sf::seconds(0.25f)}; // Tracks of the number of ticks the client is behind the
                                     // target server state. Each count is a tick delta.
    TemporalMetric m_aheadGameStateMetric{
        4 * 15, sf::seconds(0.25f)}; // Tracks the number of ticks the client is ahead the target
                                     // server state. Each count is a tick delta.
    TemporalMetric m_interpolateGameStateMetric{
        4 * 15, sf::seconds(0.25f)}; // Tracks the number of ticks away the state was interpolated
                                     // to. Each count is a tick delta.
    TemporalMetric m_stallGameStateMetric{
        4 * 15, sf::seconds(0.25f)}; // Tracks the number of ticks where the game was stalled and no
                                     // new game state was applied. Each count is a stall.
    TemporalMetric m_noGameStateMetric{
        4 * 15, sf::seconds(0.25f)}; // Tracks the number of ticks where there weren't any server
                                     // game states in the buffer.

    std::unique_ptr<NetworkingClient> m_networkingClient;
    std::unique_ptr<InputController> m_inputController;
    std::unique_ptr<RenderingController> m_renderingController;
};

#endif /* ClientGameController_hpp */
