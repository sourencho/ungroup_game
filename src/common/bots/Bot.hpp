#ifndef Bot_hpp
#define Bot_hpp

#include "../../common/objects/Mine.hpp"
#include "../../common/systems/GameObjectController.hpp"
#include "../../common/util/InputDef.hpp"
#include "../../common/util/InputUtil.hpp"
#include "../../common/util/game_settings.hpp"

#include <cstdlib>
#include <ctime>
#include <unistd.h>

#include <iostream>
#include <memory>

enum BotStrategy { Random, NearestGreedy, MostNeededGreedy, Groupie, GreedieGroupie };

class Bot {
  public:
    Bot();
    ~Bot(){};
    Bot(const Bot& temp_obj) = delete;
    Bot& operator=(const Bot& temp_obj) = delete;

    std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>
    getNearestGreedyMove(uint32_t bot_player_id, GameObjectController& goc);
    std::pair<InputDef::ReliableInput, InputDef::UnreliableInput> getRandomMove();
    std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>
    getMove(uint32_t bot_player_id, BotStrategy strategy, GameObjectController& goc);
};

#endif /* Bot_hpp */
