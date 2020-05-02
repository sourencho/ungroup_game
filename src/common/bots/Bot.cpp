#include "Bot.hpp"

Bot::Bot(){};

/* This strategy will move towards the closest mine which has a resource the bot needs to win.
 * Once it has enough of a resource, it will move to the next closest mine with a resource it needs.
 */
std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>
Bot::getGroupieMove(uint32_t bot_player_id, GameObjectController& goc) {
    auto bot_center = goc.getPlayerPosition(bot_player_id);
    auto& gc = goc.getGroupController();
    auto bot_group_id = gc.getGroupId(bot_player_id);
    auto group_ids = gc.getGroupIds();
    uint32_t nearest_joinable_group_id = -1;
    uint32_t nearest_joinable_group_dist = UINT32_MAX;

    for (auto& gid : group_ids) {
        auto& group = gc.getGroup(gid);
        if (group.isActive() && gc.getJoinable(gid) && (gid != bot_group_id)) {
            auto dist = VectorUtil::distance(bot_center, group.getCenter());
            if (dist < nearest_joinable_group_dist) {
                nearest_joinable_group_id = gid;
                nearest_joinable_group_dist = dist;
            }
        }
    }

    bool target_found = (nearest_joinable_group_id != -1);
    if (target_found) {
        auto& target_group = gc.getGroup(nearest_joinable_group_id);
        auto direction = VectorUtil::getVector(bot_center, target_group.getCenter());
        auto move = InputUtil::vectorToNearestMove(direction);
        // we only want to group with the target, so we wait to toggle joinability
        // until we're quite close to the target
        if (nearest_joinable_group_dist <
                (MIN_DISTANCE_TO_TOGGLE_JOINABLE + target_group.getRadius()) &&
            !gc.getJoinable(bot_group_id)) {
            move.first.toggle_joinable = true;
        }
        return move;
    }

    // if nothing to group with, be greedy
    return getNearestGreedyMove(bot_player_id, goc);
}

/* This strategy will move towards the closest mine which has a resource the bot needs to win.
 * Once it has enough of a resource, it will move to the next closest mine with a resource it needs.
 */
std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>
Bot::getNearestGreedyMove(uint32_t bot_player_id, GameObjectController& goc) {
    auto gso = goc.getGameStateObject();
    auto bot_center = goc.getPlayerPosition(bot_player_id);
    auto resource_controller = goc.getResourceController();
    auto& player_controller = goc.getPlayerController();
    auto current_resources = resource_controller.get(bot_player_id);
    auto mines = (goc.getMineController()).getMines();

    auto bot_player = player_controller.getPlayer(bot_player_id);
    auto& bot_win_condition = bot_player->getWinCondition();
    auto resource_counts_to_win = bot_win_condition.getResourceCountsToWin();
    std::unordered_map<uint32_t, uint32_t> mine_id_to_resource_count;
    for (auto mine : mines) {
        auto mine_id = mine->getId();
        mine_id_to_resource_count[mine_id] =
            resource_controller.get(mine_id, mine->getResourceType());
    }
    std::sort(mines.begin(), mines.end(),
              [bot_center](std::shared_ptr<Mine> mine_a, std::shared_ptr<Mine> mine_b) {
                  auto dist_a = VectorUtil::distance(bot_center, mine_a->getCenter());
                  auto dist_b = VectorUtil::distance(bot_center, mine_b->getCenter());
                  return dist_a < dist_b;
              });

    for (auto& mine : mines) {
        auto curr_mine_id = mine->getId();
        if (mine_id_to_resource_count[curr_mine_id] != 0 &&
            (current_resources[mine->getResourceType()] <
             resource_counts_to_win[mine->getResourceType()])) {
            // move towards this mine since it's closest and has a resource we need
            auto direction_to_mine = VectorUtil::getVector(bot_center, mine->getCenter());
            return InputUtil::vectorToNearestMove(direction_to_mine);
        }
    }
    // if there's no point trying to mine, then move randomly.
    return getRandomMove();
}

std::pair<InputDef::ReliableInput, InputDef::UnreliableInput> Bot::getRandomMove() {
    InputDef::ReliableInput reliable_input;
    reliable_input.setAll(false);
    // When we run many random bots, if we seed them with just the time
    // then they'll all generate the same moves. XORing with the pid is
    // an easy trick for making them have different per-process seeds.
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
Bot::getMove(uint32_t bot_player_id, BotStrategy strategy, GameObjectController& goc) {
    switch (strategy) {
        case BotStrategy::Random:
            return getRandomMove();
            break;
        case BotStrategy::NearestGreedy:
            return getNearestGreedyMove(bot_player_id, goc);
            break;
        case BotStrategy::Groupie:
            return getGroupieMove(bot_player_id, goc);
            break;
        default:
            std::cerr << "Unimplemented strategy passed to bot. Returning random move."
                      << std::endl;
            return getRandomMove();
    }
}
