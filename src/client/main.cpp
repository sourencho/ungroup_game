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
        options.add_options()("h,help", "Displays options.")(
            "x,headless", "Runs game without updating window.")("b,bot", "Runs game using a bot.")(
            "a,server-ip", "The server's IP address in CIDR notation (ex. 127.0.0.1)",
            cxxopts::value<std::string>())(
            "s,strategy", "Runs game using specified strategy. Options are Random, NearestGreedy.",
            cxxopts::value<int>());
        auto result = options.parse(argc, argv);

        bool is_help = result["help"].as<bool>();
        if (is_help) {
            std::cout << options.help() << std::endl;
            return EXIT_SUCCESS;
        }

        bool is_headless = result["headless"].as<bool>();
        bool is_bot = result["bot"].as<bool>();

        // only access the server-ip if strategy is actually present, otherwise segfaults.
        std::string server_ip = LOCALHOST_IP;
        if (result.count("server-ip")) {
            server_ip = result["server-ip"].as<std::string>();
        }
        std::cout << "Using server IP: " << server_ip << std::endl;

        BotStrategy strategy = BotStrategy::Random;
        if (result.count("strategy")) {
            strategy = static_cast<BotStrategy>(result["strategy"].as<int>());
        }

        ClientGameController client_game_controller(is_headless, is_bot, strategy, server_ip);
        client_game_controller.start();
        return EXIT_SUCCESS;
    } catch (const cxxopts::OptionException& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
