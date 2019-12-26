#include "GameObjectRenderer.hpp"

#include "../../common/util/game_settings.hpp"
#include "RenderingUtil.hpp"

GameObjectRenderer::GameObjectRenderer(ResourceStore& rs, ResourceController& rc,
                                       GroupController& gc, MineController& mc) :
    m_groupController(gc),
    m_mineController(mc), m_resourceController(rc) {
    // Initialize drawable groups
    for (size_t i = 0; i < m_groupController.getGroupIds().size(); i++) {
        m_drawableGroups.push_back(std::unique_ptr<DrawableGroup>(new DrawableGroup(rs)));
    }

    // Initialize drawable mines
    for (size_t i = 0; i < m_mineController.getMineIds().size(); i++) {
        m_drawableMines.push_back(std::unique_ptr<DrawableMine>(new DrawableMine(rs)));
    }

    // Initalize player id text
    for (size_t i = 0; i < m_groupController.getGroupIds().size(); i++) {
        sf::Text player_text("", *rs.getFont(RenderingDef::FontKey::monogram),
                             RenderingDef::PLAYER_ID_TEXT_SIZE);
        player_text.setFillColor(RenderingDef::PLAYER_ID_TEXT_COLOR);
        m_groupPlayerTexts.push_back(player_text);
    }
}

void GameObjectRenderer::draw(sf::RenderTarget& target) {
    drawGroups(target);
    drawMines(target);
}

void GameObjectRenderer::drawUI(sf::RenderWindow& window, sf::View& player_view) {
    drawGroupPlayerIds(window, player_view);
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

        drawable_mine->update(mine, m_resourceController.get(mine_id, mine.getResourceType()));
        drawable_mine->draw(target, mine.isActive());
    }
}

void GameObjectRenderer::drawGroupPlayerIds(sf::RenderWindow& window, sf::View& player_view) {
    if (!SHOW_PLAYER_IDS) {
        return;
    }
    for (size_t i = 0; i < m_groupController.getGroupIds().size(); i++) {
        uint32_t group_id = m_groupController.getGroupIds()[i];
        Group& group = m_groupController.getGroup(group_id);
        if (group.isActive()) {
            auto text = m_groupPlayerTexts[i];
            const auto& player_ids = m_groupController.getGroupPlayerIds(group_id);
            text.setString(RenderingUtil::idVecToStr(player_ids, " "));
            sf::FloatRect textRect = text.getLocalBounds();
            text.setOrigin(textRect.left + textRect.width / 2.0f,
                           textRect.top + textRect.height / 2.0f);
            text.setPosition(RenderingUtil::mapCoordToPixelScaled(
                group.getCenter(), window, player_view, GAME_SCALING_FACTOR));
            window.draw(text);
        }
    }
}