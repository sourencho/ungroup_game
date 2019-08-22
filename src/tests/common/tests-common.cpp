#include "../catch.hpp"

#include "../../common/CircleRigidBody.hpp"

SCENARIO( "CircleRigidBody step correctly", "[circle]" ) {
  sf::Vector2f position = sf::Vector2f(0.f, 0.f);
  CircleRigidBody* circle = new CircleRigidBody(0.f, position);
  GIVEN( "a velocity" ) {
    circle->setVelocity(sf::Vector2f(10.f, 10.f));
    WHEN( "step is called" ) {
      circle->step();
      REQUIRE(circle->getPosition() == sf::Vector2f(10.f, 10.f));
    }
    WHEN( "step is called many times" ) {
      for (int i=0; i < 10; i++) {
        circle->step();
      }
      REQUIRE(circle->getPosition() == sf::Vector2f(10*10.f, 10*10.f));
    }
  }
}
