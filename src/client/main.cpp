#include <cxxopts.hpp>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "../common/bots/Bot.hpp"
#include "../common/util/game_def.hpp"
#include "../common/util/game_settings.hpp"
#include "systems/ClientGameController.hpp"

int main(int argc, char** argv) {
    try {
        cxxopts::Options options("Ungroup Client",
                                 "The client for Ungroup - a game about temporary allainces.");
        options.add_options()("x,headless", "Runs game without updating window.")(
            "b,bot", "Runs game using a bot.")(
            "s,strategy", "Runs game using specified strategy. Options are Random, NearestGreedy.",
            cxxopts::value<int>());
        auto result = options.parse(argc, argv);
        bool is_headless = result["headless"].as<bool>();
        bool is_bot = result["bot"].as<bool>();
        bool strategy_opt_present = result.count("strategy");
        BotStrategy strategy = BotStrategy::Random;
        if (strategy_opt_present) {
            strategy = static_cast<BotStrategy>(result["strategy"].as<int>());
        }

        ClientGameController client_game_controller(is_headless, is_bot, strategy);
        client_game_controller.start();
        return EXIT_SUCCESS;
    } catch (const cxxopts::OptionException& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
