#include "../catch.hpp"
#include "../../common/CircleRigidBody.hpp"

SCENARIO( "CircleRigidBody changes position", "[circle]" ) {
  sf::Vector2f position = sf::Vector2f(0.f, 0.f);
  CircleRigidBody* circle = new CircleRigidBody(0.f, position);

  GIVEN( "a velocity is set on a circle" ) {
    circle->setVelocity(sf::Vector2f(10.f, 10.f));

    WHEN( "step is called once" ) {
      circle->step();

      THEN( "the circle is an increment of its velocity away from its starting position" ) {
        sf::Vector2f position = circle->getPosition();
        REQUIRE(position.x == 10.f);
        REQUIRE(position.y == 10.f);
      }
    }

    WHEN( "step is called twice without changing velocity" ) {
      int number_of_steps = 2;
      for (int i=0; i < number_of_steps; i++) {
        circle->step();
      }

      THEN( "the circle is the vector of its velocity away from its starting position" ) {
        sf::Vector2f position = circle->getPosition();
        REQUIRE(position.x == number_of_steps*10.f);
        REQUIRE(position.y == number_of_steps*10.f);
      }
    }

    WHEN( "step is called, the velocity is changed and step is called again" ) {
      circle->step();
      circle->setVelocity(sf::Vector2f(20.f, 20.f));
      circle->step();

      THEN( "the circle is the vector of its velocities away from its starting position" ) {
        sf::Vector2f position = circle->getPosition();
        REQUIRE(position.x == 30.f);
        REQUIRE(position.y == 30.f);
      }
    }

    WHEN( "step is called, the velocity is flipped and step is called again" ) {
      circle->step();
      circle->setVelocity(sf::Vector2f(-10.f, -10.f));
      circle->step();

      THEN( "the circle is the vector of its velocities away from its starting position" ) {
        sf::Vector2f position = circle->getPosition();
        REQUIRE(position.x == 0.f);
        REQUIRE(position.y == 0.f);
      }
    }
  }

  GIVEN( "a velocity is not set on a circle" ) {
    WHEN( "step is called once" ) {
      circle->step();

      THEN( "the circle's position is the same" ) {
        sf::Vector2f position = circle->getPosition();
        REQUIRE(position.x == 0.f);
        REQUIRE(position.y == 0.f);
      }
    }

    WHEN( "step is called twice" ) {
      int number_of_steps = 2;
      for (int i=0; i < number_of_steps; i++) {
        circle->step();
      }

      THEN( "the circle's position is the same" ) {
        sf::Vector2f position = circle->getPosition();
        REQUIRE(position.x == number_of_steps*0.f);
        REQUIRE(position.y == number_of_steps*0.f);
      }
    }
  }
}
