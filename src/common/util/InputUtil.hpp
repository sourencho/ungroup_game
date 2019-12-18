#ifndef InputUtil_hpp
#define InputUtil_hpp

#include "../../common/util/InputDef.hpp"
#include "../physics/VectorUtil.hpp"

namespace InputUtil {
    std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>
    vectorToNearestMove(sf::Vector2f v);
    sf::Vector2f inputToDirection(bool toggle_up, bool toggle_down, bool toggle_right,
                                  bool toggle_left, bool toggle_stop);
} // namespace InputUtil

#endif /* InputUtil_hpp */
