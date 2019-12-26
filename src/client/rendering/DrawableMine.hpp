/**
 * Renders a mine
 */

#ifndef DrawableMine_hpp
#define DrawableMine_hpp

#include <SFML/Graphics.hpp>

#include "../../common/objects/Mine.hpp"
#include "../resources/ResourceStore.hpp"
#include "DrawableCircle.hpp"

class DrawableMine : public DrawableCircle {
  public:
    explicit DrawableMine(ResourceStore& rs);
    ~DrawableMine(){};
    DrawableMine(const DrawableMine& temp_obj) = delete;
    DrawableMine& operator=(const DrawableMine& temp_obj) = delete;

    void draw(sf::RenderTarget& target, Mine& mine, uint32_t resource_count);

  private:
};

#endif /* DrawableMine_hpp */
