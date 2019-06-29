#include "../catch.hpp"

#include "../../common/Circle.hpp"

SCENARIO( "Circles move correctly", "[circle]" ) {
  sf::Vector2f position = sf::Vector2f(0.f, 0.f);
  Circle* circle = new Circle(0.f, position);
  GIVEN( "a velocity" ) {
    circle->setVelocity(sf::Vector2f(10.f, 10.f));
    WHEN( "move is called" ) {
      circle->move();
      REQUIRE(circle->getPosition() == sf::Vector2f(10.f, 10.f));
    }
    WHEN( "move is called many times" ) {
      for (int i=0; i < 10; i++) {
        circle->move();
      }
      REQUIRE(circle->getPosition() == sf::Vector2f(10*10.f, 10*10.f));
    }
  }
}
