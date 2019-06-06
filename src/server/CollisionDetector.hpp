#ifndef CollisionDetector_hpp
#define CollisionDetector_hpp

#include <stdio.h>
#include "Group.hpp"
#include "../common/util.hpp"
#include "../common/GroupShape.hpp"

class CollisionDetector
{
    public:
        CollisionDetector();
        template<typename T> void detectAndHandleShapeCollisions(const std::vector<T*>& shapes);
        void handleGroupShapeCollision(GroupShape* group_a, GroupShape* group_b);
        void detectAndHandleGroupShapeCollisoin(std::vector<GroupShape*> group_shapes);
};

// TODO: Move this into a tpp file?
template<class Group> void CollisionDetector::detectAndHandleShapeCollisions(const std::vector<Group*>& shapes) {
    // Cast Groups to GroupShapes
    std::vector<GroupShape*> group_shapes;
    for (auto group: shapes) {
        group_shapes.push_back((GroupShape*) group);
    }

    // Handle GroupShape collision
    detectAndHandleGroupShapeCollisoin(group_shapes);
}

#endif /* CollisionDetector_hpp */
