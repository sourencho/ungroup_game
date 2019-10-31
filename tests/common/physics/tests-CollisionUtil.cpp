#include <catch.hpp>
#include <cmath>

#include "../../../src/common/physics/CollisionUtil.hpp"

TEST_CASE("Intersection", "[common][physics][VectorUtil][areIntersecting]") {
    CircleRigidBody circle_a = CircleRigidBody(0, 10.f, {10.f, 10.f}, 1.f);
    CircleRigidBody circle_b = CircleRigidBody(1, 10.f, {15.f, 15.f}, 1.f);
    CircleRigidBody circle_c = CircleRigidBody(2, 10.f, {30.f, 30.f}, 1.f);

    REQUIRE(CollisionUtil::areIntersecting(circle_a, circle_b) == true);
    REQUIRE(CollisionUtil::areIntersecting(circle_a, circle_c) == false);
}

TEST_CASE("Collision", "[common][physics][VectorUtil][getCollision]") {
    SECTION("Two movable circles") {
        SECTION("Horizontal collision") {
            CircleRigidBody movable_circle_a = CircleRigidBody(0, 10.f, {0.f, 0.f}, 1.f, true);
            CircleRigidBody movable_circle_b = CircleRigidBody(1, 12.f, {16.f, -2.f}, 1.f, true);

            Collision collision = CollisionUtil::getCollision(movable_circle_a, movable_circle_b);
            REQUIRE(CollisionUtil::areIntersecting(movable_circle_a, movable_circle_b) == true);

            REQUIRE(collision.ids.first == 0);
            REQUIRE(collision.ids.second == 1);
            REQUIRE(collision.position.x == 18.f);
            REQUIRE(collision.position.y == 10.f);
            REQUIRE(collision.normal.x == -1.f);
            REQUIRE(collision.normal.y == 0.f);
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
            CircleRigidBody movable_circle_a = CircleRigidBody(0, 10.f, {0.f, 0.f}, 1.f, true);
            CircleRigidBody unmovable_circle_b = CircleRigidBody(1, 10.f, {16.f, 0.f}, 1.f, false);

            Collision collision = CollisionUtil::getCollision(movable_circle_a, unmovable_circle_b);
            REQUIRE(CollisionUtil::areIntersecting(movable_circle_a, unmovable_circle_b) == true);

            SECTION("Outward horizontal collision correct") {
                REQUIRE(collision.ids.first == 0);
                REQUIRE(collision.ids.second == 1);
                REQUIRE(collision.position.x == 16.f);
                REQUIRE(collision.position.y == 10.f);
                REQUIRE(collision.normal.x == -1.f);
                REQUIRE(collision.normal.y == 0.f);
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
        CircleRigidBody movable_circle_a = CircleRigidBody(0, 10.f, {0.f, 0.f}, 1.f, true);
        CircleRigidBody movable_circle_b = CircleRigidBody(1, 10.f, {11.f, 11.f}, 1.f, true);
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

// Uses "Explore more 2" in https://www.ck12.org/book/CBSE_Physics_Book_Class_XI/section/5.12/
TEST_CASE("Impulses", "[common][physics][VectorUtil][getImpulses]") {
    CircleRigidBody movable_circle_a = CircleRigidBody(0, 10.f, {0.f, 0.f}, 0.045f, true);
    CircleRigidBody movable_circle_b = CircleRigidBody(1, 10.f, {20.f, 0.f}, .17f, true);
    movable_circle_a.setActive(true);
    movable_circle_b.setActive(true);

    SECTION("Horiontal collision between moving and static circles") {
        movable_circle_a.setVelocity({42.f, 0.f});
        movable_circle_b.setVelocity({0.f, 0.f});
        Collision collision = {
            .ids = std::make_pair(0, 1),
            .position = {10.f, 20.f},
            .normal = {-1.f, 0.f},
            .resolution = std::make_pair(sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 0.f)),
        };
        auto impulses = CollisionUtil::getImpulses(movable_circle_a, movable_circle_b, collision);

        movable_circle_a.applyImpulse(impulses.first);
        movable_circle_b.applyImpulse(impulses.second);

        REQUIRE((int)(VectorUtil::length(movable_circle_a.getVelocity()) * 100.f) == 2441);
        REQUIRE((int)(VectorUtil::length(movable_circle_b.getVelocity()) * 100.f) == 1758);
    }
}