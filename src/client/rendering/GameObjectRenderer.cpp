#include "GameObjectRenderer.hpp"

#include "RenderingDef.hpp"
#include "RenderingUtil.hpp"

GameObjectRenderer::GameObjectRenderer(ResourceStore& rs, ResourceController& rc,
                                       GroupController& gc, MineController& mc,
                                       PlayerController& pc, uint32_t player_id) :
    m_groupController(gc),
    m_mineController(mc), m_playerController(pc), m_resourceController(rc), m_playerId(player_id) {
    // Init drawable groups
    for (size_t i = 0; i < m_groupController.getGroupIds().size(); i++) {
        m_drawableGroups.push_back(std::unique_ptr<DrawableGroup>(new DrawableGroup(rs)));
    }

    // Init drawable mines
    for (size_t i = 0; i < m_mineController.getMineIds().size(); i++) {
        m_drawableMines.push_back(std::unique_ptr<DrawableMine>(new DrawableMine(rs)));
    }

    // Init player id text
    for (size_t i = 0; i < m_groupController.getGroupIds().size(); i++) {
        sf::Text player_text("", *rs.getFont(RenderingDef::FontKey::monogram),
                             RenderingDef::PLAYER_ID_TEXT_SIZE);
        player_text.setFillColor(RenderingDef::PLAYER_ID_TEXT_COLOR);
        m_groupPlayerTexts.push_back(player_text);
    }

    // Init mine directions
    for (uint32_t mine_id : m_mineController.getMineIds()) {
        Mine& mine = m_mineController.getMine(mine_id);
        sf::Sprite direction_sprite(
            *rs.getTexture(RenderingDef::RESOURCE_TEXTURE_KEYS[mine.getResourceType()]).get());
        direction_sprite.setScale({2.5f, 2.5f});
        direction_sprite.setColor(RenderingDef::RESOURCE_COLORS[mine.getResourceType()]);
        direction_sprite.setOrigin(direction_sprite.getTexture()->getSize().x / 2.f,
                                   direction_sprite.getTexture()->getSize().y / 2.f);
        m_mineDirections.push_back(direction_sprite);
    }
}

void GameObjectRenderer::draw(sf::RenderTarget& target) {
    drawGroups(target);
    drawMines(target);
}

void GameObjectRenderer::drawUI(sf::RenderWindow& window, sf::View& player_view) {
    drawGroupPlayerIds(window, player_view);
    drawMineDirections(window, player_view);
}

void GameObjectRenderer::drawGroups(sf::RenderTarget& target) {
    for (size_t i = 0; i < m_groupController.getGroupIds().size(); i++) {
        uint32_t group_id = m_groupController.getGroupIds()[i];
        auto& drawable_group = m_drawableGroups[i];
        Group& group = m_groupController.getGroup(group_id);

        drawable_group->draw(
            target, group, m_groupController.getJoinable(group_id),
            m_groupController.getUngroup(group_id), m_groupController.getPlayerDirections(group_id),
            m_groupController.getPlayerIntents(group_id), m_groupController.getResources(group_id));
    }
}

void GameObjectRenderer::drawMines(sf::RenderTarget& target) {
    for (size_t i = 0; i < m_mineController.getMineIds().size(); i++) {
        uint32_t mine_id = m_mineController.getMineIds()[i];
        auto& drawable_mine = m_drawableMines[i];
        Mine& mine = m_mineController.getMine(mine_id);

        drawable_mine->draw(target, mine, m_resourceController.get(mine_id, mine.getResourceType()),
                            m_resourceController.get(mine_id));
    }
}

void GameObjectRenderer::drawGroupPlayerIds(sf::RenderWindow& window, sf::View& player_view) {
    if (!RenderingDef::SHOW_PLAYER_IDS) {
        return;
    }
    for (size_t i = 0; i < m_groupController.getGroupIds().size(); i++) {
        uint32_t group_id = m_groupController.getGroupIds()[i];
        Group& group = m_groupController.getGroup(group_id);
        if (group.isActive()) {
            auto& text = m_groupPlayerTexts[i];
            const auto& player_ids = m_groupController.getGroupPlayerIds(group_id);
            text.setString(RenderingUtil::idVecToStr(player_ids, " "));
            sf::FloatRect textRect = text.getLocalBounds();
            text.setOrigin(textRect.left + textRect.width / 2.0f,
                           textRect.top + textRect.height / 2.0f);
            text.setPosition(RenderingUtil::mapCoordToPixelScaled(
                group.getCenter(), window, player_view, RenderingDef::GAME_SCALING_FACTOR));
            window.draw(text);
        }
    }
}

void GameObjectRenderer::drawMineDirections(sf::RenderWindow& window, sf::View& player_view) {
    if (!RenderingDef::SHOW_MINE_DIRECTIONS) {
        return;
    }

    sf::Vector2f view_size(player_view.getSize().x / RenderingDef::GAME_SCALING_FACTOR.x,
                           player_view.getSize().y / RenderingDef::GAME_SCALING_FACTOR.y);

    sf::Vector2f view_center(player_view.getCenter().x / RenderingDef::GAME_SCALING_FACTOR.x,
                             player_view.getCenter().y / RenderingDef::GAME_SCALING_FACTOR.y);

    sf::FloatRect view_rect({
        view_center.x - view_size.x / 2.f,
        view_center.y - view_size.y / 2.f,
        view_size.x,
        view_size.y,
    });

    for (size_t i = 0; i < m_mineController.getMineIds().size(); i++) {
        uint32_t mine_id = m_mineController.getMineIds()[i];
        Mine& mine = m_mineController.getMine(mine_id);
        const sf::Vector2f& mine_center = mine.getCenter();

        // Only show direction if it's the player's intended resource
        if (mine.getResourceType() != m_playerController.getPlayer(m_playerId)->getIntent()) {
            continue;
        }

        // Only show direction if mine is out of view
        if (!CollisionUtil::areIntersecting(mine.getRigidBody(), view_rect)) {
            float left = view_rect.left;
            float right = view_rect.left + view_rect.width;
            float top = view_rect.top;
            float bottom = view_rect.top + view_rect.height;

            auto& direction = m_mineDirections[i];
            const sf::FloatRect direction_size = direction.getGlobalBounds();
            sf::Vector2f direction_position;

            Orientation orientation = CollisionUtil::getOrientation(mine_center, view_rect);
            switch (orientation) {
                case Orientation::above:
                    direction_position = {
                        VectorUtil::clamp(mine_center.x,
                                          left + RenderingDef::MINE_DIRECTION_PADDING,
                                          right - RenderingDef::MINE_DIRECTION_PADDING),
                        top + RenderingDef::MINE_DIRECTION_PADDING};
                    break;
                case Orientation::right:
                    direction_position = {
                        right - RenderingDef::MINE_DIRECTION_PADDING,
                        VectorUtil::clamp(mine_center.y, top + RenderingDef::MINE_DIRECTION_PADDING,
                                          bottom - RenderingDef::MINE_DIRECTION_PADDING)};
                    break;
                case Orientation::below:
                    direction_position = {
                        VectorUtil::clamp(mine_center.x,
                                          left + RenderingDef::MINE_DIRECTION_PADDING,
                                          right - RenderingDef::MINE_DIRECTION_PADDING),
                        bottom - RenderingDef::MINE_DIRECTION_PADDING};
                    break;
                case Orientation::left:
                    direction_position = {
                        left + RenderingDef::MINE_DIRECTION_PADDING,
                        VectorUtil::clamp(mine_center.y, top + RenderingDef::MINE_DIRECTION_PADDING,
                                          bottom - RenderingDef::MINE_DIRECTION_PADDING)};
                    break;
                default:
                    break;
            }
            direction.setPosition(RenderingUtil::mapCoordToPixelScaled(
                direction_position, window, player_view, RenderingDef::GAME_SCALING_FACTOR));
            window.draw(direction);
        }
    }
}