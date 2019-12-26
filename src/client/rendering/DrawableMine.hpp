/**
 * Renders a mine
 */

#ifndef DrawableMine_hpp
#define DrawableMine_hpp

#include <SFML/Graphics.hpp>

#include "../../common/objects/Mine.hpp"
#include "../resources/ResourceStore.hpp"

class DrawableMine {
  public:
    explicit DrawableMine(ResourceStore& rs);
    ~DrawableMine(){};
    DrawableMine(const DrawableMine& temp_obj) = delete;
    DrawableMine& operator=(const DrawableMine& temp_obj) = delete;

    void update(Mine& mine, uint32_t resource_count);
    void draw(sf::RenderTarget& target, bool active);

  private:
    sf::CircleShape m_circleShape;
    ResourceStore& m_resourceStore;
};

#endif /* DrawableMine_hpp */
