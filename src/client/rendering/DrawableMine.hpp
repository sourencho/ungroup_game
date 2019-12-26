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

    void draw(sf::RenderTarget& target, Mine& mine, uint32_t resource_count,
              const std::array<uint32_t, RESOURCE_TYPE_COUNT>& resource_counts);

  private:
    float m_resourceCounts[RESOURCE_TYPE_COUNT] = {0}; // Used to pass resource counts to shader
};

#endif /* DrawableMine_hpp */
