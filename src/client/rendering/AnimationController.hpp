#ifndef AnimationController_hpp
#define AnimationController_hpp

#include "../../common/events/Event.hpp"
#include "../../common/resources/ResourceStore.hpp"
#include "AnimatedSprite.hpp"

class AnimationController {
  public:
    explicit AnimationController(ResourceStore& rs);
    ~AnimationController(){};
    AnimationController(const AnimationController& temp_obj) = delete;
    AnimationController& operator=(const AnimationController& temp_obj) = delete;

    void add(std::unique_ptr<AnimatedSprite> animated_sprite);
    void step(sf::Uint32 delta_ms);
    void draw(sf::RenderTarget& target);
    const size_t getCount() const {
        return m_animatedSprites.size();
    }

    void handleCollisionEvent(std::shared_ptr<Event> event);

  private:
    /**
     * Register callbacks for events.
     */
    void addEventListeners();
    void createCollisionAnimation(const sf::Vector2f& collision_position);

    std::vector<std::unique_ptr<AnimatedSprite>> m_animatedSprites;
    ResourceStore& m_resourceStore;
};

#endif /* AnimationController_hpp */
