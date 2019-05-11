//
//  util.cpp
//  ungroup_sfml
//
//  Created by Souren Papazian on 5/8/19.
//  Copyright © 2019 Souren Papazian. All rights reserved.
//

#include "util.hpp"
#include <cmath>

sf::Vector2f normalize(const sf::Vector2f& source)
{
    float length = std::sqrt((source.x * source.x) + (source.y * source.y));
    if (length != 0)
        return sf::Vector2f(source.x / length, source.y / length);
    else
        return source;
}
