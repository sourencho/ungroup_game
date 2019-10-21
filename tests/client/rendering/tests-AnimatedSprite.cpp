#include <catch.hpp>

#include "../../../src/client/rendering/AnimatedSprite.hpp"

SCENARIO("AnimatedSprite is created and animation completed",
         "[common][rendering][AnimatedSprite]") {
    GIVEN("An AnimatedSprite is created with duration 200") {
        sf::Texture texture = sf::Texture();
        texture.loadFromFile("resources/images/test.png");
        AnimatedSprite animated_sprite =
            AnimatedSprite(texture, {3, 3}, 200, {100.f, 100.f}, {1.f, 1.f});
        WHEN("is updated one time at 100ms") {
            animated_sprite.update(100);
            REQUIRE(animated_sprite.isDone() == false);
        }
        WHEN("is updated two times at 150ms") {
            animated_sprite.update(150);
            animated_sprite.update(150);
            REQUIRE(animated_sprite.isDone() == true);
        }
    }
}
