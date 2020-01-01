#include <cxxopts.hpp>
#include <iostream>

#include "../common/systems/LevelController.hpp"
#include "../common/util/game_settings.hpp"
#include "systems/ServerGameController.hpp"

int main(int argc, char** argv) {
    try {
        cxxopts::Options options("Ungroup Server",
                                 "The server for Ungroup - a game about temporary allainces.");
        options.add_options()("l,level", "Loads a level. Options are mine_ring, empty.",
                              cxxopts::value<int>());
        auto result = options.parse(argc, argv);

        bool level_opt_present = result.count("level");
        LevelKey level = LevelKey::mine_ring;
        if (level_opt_present) {
            level = static_cast<LevelKey>(result["level"].as<int>());
        }

        ServerGameController server_game_controller(level);
        server_game_controller.start();
        return EXIT_SUCCESS;
    } catch (const cxxopts::OptionException& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
