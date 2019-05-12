#ifndef CollisionDetector_hpp
#define CollisionDetector_hpp

#include <stdio.h>
#include "Group.hpp"

class CollisionDetector
{
    public:
        CollisionDetector();
        void detectGroupCollisions(const std::vector<Group*> groups);
};

#endif /* CollisionDetector_hpp */
