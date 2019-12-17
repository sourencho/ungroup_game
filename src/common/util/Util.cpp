#include "Util.hpp"

std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>
Util::vectorToNearestMove(sf::Vector2f v) {
    InputDef::ReliableInput reliable_input;
    reliable_input.setAll(false);

    auto directions = VectorUtil::DIRECTIONS;
    std::sort(directions.begin(), directions.end(), [v](sf::Vector2f dir_a, sf::Vector2f dir_b) {
        return VectorUtil::cos_sim(dir_a, v) > VectorUtil::cos_sim(dir_b, v);
    });

    auto closestAllowedDirection = directions[0];
    bool go_left = (closestAllowedDirection.x == -1);
    bool go_right = (closestAllowedDirection.x == 1);
    bool go_up = (closestAllowedDirection.y == -1);
    bool go_down = (closestAllowedDirection.y == 1);
    InputDef::UnreliableInput unreliable_input = {
        .toggle_up = go_up,
        .toggle_down = go_down,
        .toggle_right = go_right,
        .toggle_left = go_left,
        .toggle_stop = false,
    };
    return std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>(reliable_input,
                                                                         unreliable_input);
}

sf::Vector2f Util::inputToDirection(bool toggle_up, bool toggle_down, bool toggle_right,
                                    bool toggle_left, bool toggle_stop) {
    if (toggle_stop) {
        return sf::Vector2f(0.f, 0.f);
    }

    sf::Vector2f direction = sf::Vector2f(0.f, 0.f);
    if (toggle_up) {
        direction += sf::Vector2f(0.f, -1.f);
    }
    if (toggle_down) {
        direction += sf::Vector2f(0.f, 1.f);
    }
    if (toggle_left) {
        direction += sf::Vector2f(-1.f, 0.f);
    }
    if (toggle_right) {
        direction += sf::Vector2f(1.f, 0.f);
    }
    return VectorUtil::normalize(direction);
};
