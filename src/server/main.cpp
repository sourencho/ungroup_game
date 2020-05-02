#include <cxxopts.hpp>
#include <iostream>

#include "../common/systems/LevelController.hpp"
#include "../common/util/game_settings.hpp"
#include "systems/ServerGameController.hpp"

// Load constants from config
// const GameSettings GAME_SETTINGS =

int main(int argc, char** argv) {
    try {
        cxxopts::Options options("Ungroup Server",
                                 "The server for Ungroup - a game about temporary allainces.");
        options.add_options()("h,help", "Displays options.")("p,port", "The TCP port to listen on.",
                                                             cxxopts::value<int>())(
            "l,level", "Loads a level. Options are mine_ring, empty.", cxxopts::value<int>());
        auto result = options.parse(argc, argv);

        bool is_help = result["help"].as<bool>();
        if (is_help) {
            std::cout << options.help() << std::endl;
            return EXIT_SUCCESS;
        }

        bool level_opt_present = result.count("level");
        LevelKey level = LevelKey::mine_ring;
        if (level_opt_present) {
            level = static_cast<LevelKey>(result["level"].as<int>());
        }

        bool tcp_port_opt_present = result.count("port");
        uint32_t tcp_port = GAME_SETTINGS.SERVER_TCP_PORT;
        if (tcp_port_opt_present) {
            tcp_port = result["port"].as<int>();
        }
        ServerGameController server_game_controller(level, tcp_port);
        server_game_controller.start();
        return EXIT_SUCCESS;
    } catch (const cxxopts::OptionException& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
