#include "Bot.hpp"

Bot::Bot() {};

/* This strategy will move towards the closest mine which has a resource the bot needs to win.
 * Once it has enough of a resource, it will move to the next closest mine with a resource it needs. */
std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>
Bot::getNearestGreedyMove(std::vector<std::shared_ptr<Mine>> mines,
                std::array<uint32_t, RESOURCE_TYPE_COUNT> current_resources,
                sf::Vector2f bot_center,
                std::unordered_map<uint32_t, uint32_t> mine_id_to_resource_count
                ) {
    std::sort(mines.begin(), mines.end(),
            [bot_center] (std::shared_ptr<Mine> mine_a, std::shared_ptr<Mine> mine_b) {
                auto dist_a = VectorUtil::distance(bot_center, mine_a->getCenter());
                auto dist_b = VectorUtil::distance(bot_center, mine_b->getCenter());
                return dist_a < dist_b;
            }
    );

    for ( auto mine : mines ) {
        auto curr_mine_id = mine->getId();
        if (mine_id_to_resource_count[curr_mine_id] != 0 &&
                current_resources[mine->getResourceType()] != WIN_CONDITION[mine->getResourceType()]) {
            // move towards this mine since it's closest and has a resource we need
            auto direction_to_mine = VectorUtil::getVector(bot_center, mine->getCenter());
            return Util::vectorToNearestMove(direction_to_mine);
        }
    }
    // if there's no point trying to mine, then move randomly.
    return getRandomMove();
}

std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>
Bot::getRandomMove() {
    InputDef::ReliableInput reliable_input;
    reliable_input.setAll(false);
    /*  When we run many random bots, if we seed them with just the time
        then they'll all generate the same moves. XORing with the pid is
        an easy trick for making them have different per-process seeds. */
    srand(time(0) ^ getpid());
    bool up = bool(rand() % 2);
    bool down = !up ? bool(rand() % 2) : false;
    bool right = bool(rand() % 2);
    bool left = !up ? bool(rand() % 2) : false;
    bool stop = bool(rand() % 2);
    InputDef::UnreliableInput unreliable_input = {
        .toggle_up = up,
        .toggle_down = down,
        .toggle_right = right,
        .toggle_left = left,
        .toggle_stop = stop,
    };
    return std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>(reliable_input,
                                                                         unreliable_input);
}

std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>
Bot::getMove(BotStrategy strategy, std::vector<std::shared_ptr<Mine>> mines,
                std::array<uint32_t, RESOURCE_TYPE_COUNT> current_resources,
                sf::Vector2f bot_center,
                std::unordered_map<uint32_t, uint32_t> mine_id_to_resource_count
                ) {
    switch (strategy) {
        case BotStrategy::Random:
            return getRandomMove();
            break;
        case BotStrategy::NearestGreedy:
            return getNearestGreedyMove(mines, current_resources, bot_center, mine_id_to_resource_count);
            break;
        default:
            std::cerr << "Unimplemented strategy passed to bot. Returning random move." << std::endl;
            return getRandomMove();
    }
}
