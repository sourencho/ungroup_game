#include <catch.hpp>

#include "../../../src/client/rendering/AnimationController.hpp"
#include "../../../src/common/resources/ResourceStore.hpp"

SCENARIO("AnimatedSprite is added to AnimationController and removed on completion",
         "[common][rendering][AnimationController]") {
    GIVEN("An AnimatedSprited sprite with duration 200 is added") {
        sf::Texture texture;
        texture.loadFromFile("resources/images/test.png");
        auto animated_sprite = std::unique_ptr<AnimatedSprite>(
            new AnimatedSprite(texture, {6, 1}, 200, {100.f, 100.f}, {1.f, 1.f}));
        ResourceStore resource_store;
        AnimationController animation_controller(resource_store);
        animation_controller.add(std::move(animated_sprite));
        WHEN("step one time at 100ms") {
            animation_controller.update(100);
            REQUIRE(animation_controller.getCount() == 1);
        }
        WHEN("is updated two times at 150ms") {
            animation_controller.update(150);
            animation_controller.update(150);
            REQUIRE(animation_controller.getCount() == 0);
        }
    }
}
