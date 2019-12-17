#ifndef Util_hpp
#define Util_hpp

#include "../../common/util/InputDef.hpp"
#include "../physics/VectorUtil.hpp"

namespace Util {
    std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>
    vectorToNearestMove(sf::Vector2f v);
    sf::Vector2f inputToDirection(bool toggle_up, bool toggle_down, bool toggle_right,
                                  bool toggle_left, bool toggle_stop);
} // namespace Util

#endif /* Util_hpp */
