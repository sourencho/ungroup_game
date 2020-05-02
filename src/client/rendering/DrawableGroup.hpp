/**
 * Renders a group
 */

#ifndef DrawableGroup_hpp
#define DrawableGroup_hpp

#include <SFML/Graphics.hpp>

#include <vector>

#include "../../common/objects/Group.hpp"
#include "../resources/ResourceStore.hpp"
#include "DirectionArrows.hpp"
#include "DirectionLines.hpp"
#include "DrawableCircle.hpp"

class DrawableGroup : public DrawableCircle {
  public:
    explicit DrawableGroup(ResourceStore& rs);
    ~DrawableGroup(){};
    DrawableGroup(const DrawableGroup& temp_obj) = delete;
    DrawableGroup& operator=(const DrawableGroup& temp_obj) = delete;

    void draw(sf::RenderTarget& target, Group& group, bool joinable, bool ungroup,
              const std::vector<std::shared_ptr<Player>>& players,
              const std::array<uint32_t, RESOURCE_TYPE_COUNT>& resource_counts, uint32_t player_id);

  private:
    void drawDirectionLines(sf::RenderTarget& target, Group& group,
                            const std::vector<std::shared_ptr<Player>>& players);

    void drawDirectionArrows(sf::RenderTarget& target, Group& group,
                             const std::vector<std::shared_ptr<Player>>& players,
                             uint32_t player_id);

    void drawGroup(sf::RenderTarget& target, Group& group, bool joinable, bool ungroup,
                   const std::array<uint32_t, RESOURCE_TYPE_COUNT>& resource_counts);

    DirectionArrows m_directionArrow;
    DirectionLines m_directionLines;
    float m_resourceCounts[RESOURCE_TYPE_COUNT] = {0}; // Used to pass resource counts to shader
};

#endif /* DrawableGroup_hpp */
