#include <catch.hpp>
#include <cmath>

#include "../../../src/common/physics/CollisionUtil.hpp"

TEST_CASE("Intersection", "[common][physics][VectorUtil][areIntersecting]") {
    CircleRigidBody circle_a = CircleRigidBody(0, 10.f, {10.f, 10.f});
    CircleRigidBody circle_b = CircleRigidBody(1, 10.f, {15.f, 15.f});
    CircleRigidBody circle_c = CircleRigidBody(2, 10.f, {30.f, 30.f});

    REQUIRE(CollisionUtil::areIntersecting(circle_a, circle_b) == true);
    REQUIRE(CollisionUtil::areIntersecting(circle_a, circle_c) == false);
}

TEST_CASE("Collision", "[common][physics][VectorUtil][getCollision]") {
    SECTION("Two movable circles") {
        SECTION("Horizontal collision") {
            CircleRigidBody movable_circle_a = CircleRigidBody(0, 10.f, {0.f, 0.f}, true);
            CircleRigidBody movable_circle_b = CircleRigidBody(1, 10.f, {16.f, 0.f}, true);

            Collision collision = CollisionUtil::getCollision(movable_circle_a, movable_circle_b);
            REQUIRE(CollisionUtil::areIntersecting(movable_circle_a, movable_circle_b) == true);

            REQUIRE(collision.ids.first == 0);
            REQUIRE(collision.ids.second == 1);
            REQUIRE(collision.position.x == 18.f);
            REQUIRE(collision.position.y == 10.f);
            REQUIRE(collision.direction.x == 1.f);
            REQUIRE(collision.direction.y == 0.f);
            REQUIRE(collision.resolution.first.x == -2.f);
            REQUIRE(collision.resolution.first.y == 0.f);
            REQUIRE(collision.resolution.second.x == 2.f);
            REQUIRE(collision.resolution.second.y == 0.f);

            SECTION("Not intersecting after applying collision resolution") {
                movable_circle_a.setActive(true);
                movable_circle_b.setActive(true);
                movable_circle_a.move(collision.resolution.first);
                movable_circle_b.move(collision.resolution.second);
                REQUIRE(CollisionUtil::areIntersecting(movable_circle_a, movable_circle_b) ==
                        false);
            }
        }
    }
    SECTION("Movable and unmovable circles") {
        SECTION("Horizontal collision") {
            CircleRigidBody movable_circle_a = CircleRigidBody(0, 10.f, {0.f, 0.f}, true);
            CircleRigidBody unmovable_circle_b = CircleRigidBody(1, 10.f, {16.f, 0.f}, false);

            Collision collision = CollisionUtil::getCollision(movable_circle_a, unmovable_circle_b);
            REQUIRE(CollisionUtil::areIntersecting(movable_circle_a, unmovable_circle_b) == true);

            SECTION("Outward horizontal collision correct") {
                REQUIRE(collision.ids.first == 0);
                REQUIRE(collision.ids.second == 1);
                REQUIRE(collision.position.x == 16.f);
                REQUIRE(collision.position.y == 10.f);
                REQUIRE(collision.direction.x == 1.f);
                REQUIRE(collision.direction.y == 0.f);
                REQUIRE(collision.resolution.first.x == -4.f);
                REQUIRE(collision.resolution.first.y == 0.f);
                REQUIRE(collision.resolution.second.x == 0.f);
                REQUIRE(collision.resolution.second.y == 0.f);

                SECTION("Not intersecting after applying collision resolution") {
                    movable_circle_a.setActive(true);
                    unmovable_circle_b.setActive(true);
                    movable_circle_a.move(collision.resolution.first);
                    unmovable_circle_b.move(collision.resolution.second);
                    REQUIRE(CollisionUtil::areIntersecting(movable_circle_a, unmovable_circle_b) ==
                            false);
                }
            }
        }
    }

    SECTION("Angular collision") {
        CircleRigidBody movable_circle_a = CircleRigidBody(0, 10.f, {0.f, 0.f}, true);
        CircleRigidBody movable_circle_b = CircleRigidBody(1, 10.f, {11.f, 11.f}, true);
        Collision collision = CollisionUtil::getCollision(movable_circle_a, movable_circle_b);
        REQUIRE(CollisionUtil::areIntersecting(movable_circle_a, movable_circle_b) == true);

        SECTION("Not intersecting after applying collision resolution") {
            movable_circle_a.setActive(true);
            movable_circle_b.setActive(true);
            movable_circle_a.move(collision.resolution.first);
            movable_circle_b.move(collision.resolution.second);
            REQUIRE(CollisionUtil::areIntersecting(movable_circle_a, movable_circle_b) == false);
        }
    }
}