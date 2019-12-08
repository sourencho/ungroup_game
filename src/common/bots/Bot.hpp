#ifndef Bot_hpp
#define Bot_hpp

#include "../../common/util/InputDef.hpp"
#include "../../common/util/Util.hpp"
#include "../../common/objects/Mine.hpp"
#include "../../common/util/game_settings.hpp"

#include <ctime>
#include <cstdlib>
#include <unistd.h>

#include <memory>
#include <iostream>

enum BotStrategy {
    Random, NearestGreedy, MostNeededGreedy, Groupie, GreedieGroupie
};

class Bot {
  public:
    Bot();
    ~Bot(){};
    Bot(const Bot& temp_obj) = delete;
    Bot& operator=(const Bot& temp_obj) = delete;

    std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>
        getNearestGreedyMove(std::vector<std::shared_ptr<Mine>> mines,
                std::array<uint32_t, RESOURCE_TYPE_COUNT> current_resources,
                sf::Vector2f bot_center,
                std::unordered_map<uint32_t, uint32_t> mine_id_to_resource_count
                );
    std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>
        getRandomMove();
    std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>
        getMove(BotStrategy strat, std::vector<std::shared_ptr<Mine>> mines,
                std::array<uint32_t, RESOURCE_TYPE_COUNT> current_resources,
                sf::Vector2f bot_center,
                std::unordered_map<uint32_t, uint32_t> mine_id_to_resource_count
                );
};

#endif /* Bot_hpp */
