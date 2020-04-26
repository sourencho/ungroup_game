#include "TerminalRenderingController.hpp"

TerminalRenderingController::TerminalRenderingController() : m_termboxTextOutputArea(10) {
    // termbox setup
    int ret = tb_init();
    if (ret) {
        fprintf(stderr, "tb_init() failed with error code %d\n", ret);
    }

    m_termboxPlayerTable.addColumn("Player ID");
    m_termboxPlayerTable.addColumn("UPS");
    m_termboxPlayerTable.addColumn("Avg Drift");
    m_termboxPlayerTable.addColumn("Direction");
    m_termboxPlayerTable.addColumn("Joinable");
    m_termboxServerStatsTable.addColumn("Steps per second");
    m_termboxServerStatsTable.addColumn("Updates per second");
    m_termboxServerStatsTable.addColumn("Broadcasts per second");
    m_termboxTextOutputArea.addTitle("Ungroup Server Output");
}

void TerminalRenderingController::preUpdate() {
    // collect inputs from terminal window
    struct tb_event ev;
    // non-blocking insta-peek of input
    if (tb_peek_event(&ev, 0) > 0) {
        switch (ev.type) {
            case TB_EVENT_KEY:
                switch (ev.key) {
                    case TB_KEY_ESC:
                        if (!m_shutting_down) {
                            tb_shutdown();
                            exit(0);
                        }
                        m_shutting_down = true;
                        break;
                }
                break;
        }
    }
}

void TerminalRenderingController::update() {
    // noop
}

void TerminalRenderingController::postUpdate() {
    // noop
}

void TerminalRenderingController::addWinner(const std::string& winner_player_id) {
    m_termboxTextOutputArea.addLine("We have a winner!");
    m_termboxTextOutputArea.addLine("Congratulations: " + winner_player_id);
}

void TerminalRenderingController::draw(
    std::unordered_map<uint32_t, float> player_ids_to_updates_rates,
    std::unordered_map<uint32_t, float> player_ids_to_avg_tick_drifts,
    float broadcast_game_state_rate, std::vector<PlayerUpdate> player_updates, float game_step_rate,
    float game_update_rate) {
    // clear out internal termbox screen buffer
    tb_clear();

    // output area will be half of vertical space
    m_termboxTextOutputArea.setMaxHeight(tb_height() / 2);
    m_termboxTextOutputArea.setLines();

    // server stats will render halfway down the screen
    m_termboxServerStatsTable.setOrigin(0, (tb_height() / 2) + 1);
    m_termboxServerStatsTable.setColHeaders();
    m_termboxServerStatsTable.setCell(std::to_string(game_step_rate), 1, 0);
    m_termboxServerStatsTable.setCell(std::to_string(game_update_rate), 1, 1);
    m_termboxServerStatsTable.setCell(std::to_string(broadcast_game_state_rate), 1, 2);

    // player table will be on right half of screen
    m_termboxPlayerTable.setOrigin(tb_width() / 2, 0);
    m_termboxPlayerTable.setColHeaders();

    for (auto pu : player_updates) {
        uint32_t curr_row = 1;
        uint32_t curr_col = 0;
        if (pu.is_active) {
            // update player ID column
            auto player_id = std::to_string(pu.player_id);
            m_termboxPlayerTable.setCell(player_id, curr_row, curr_col++);

            // update UPS column
            if (player_ids_to_updates_rates.count(pu.player_id) != 0) {
                float rate = player_ids_to_updates_rates[pu.player_id];
                // ups = updates per sec
                auto ups = std::to_string(rate);
                ups.resize(2);
                m_termboxPlayerTable.setCell(ups, curr_row, curr_col++);
            }

            // update drift column
            if (player_ids_to_avg_tick_drifts.count(pu.player_id) != 0) {
                float rate = player_ids_to_avg_tick_drifts[pu.player_id];
                auto drift = std::to_string(rate);
                drift.resize(2);
                m_termboxPlayerTable.setCell(drift, curr_row, curr_col++);
            }

            // update direction column
            auto x_dir = std::to_string(pu.direction.x);
            x_dir.resize(4);
            auto y_dir = std::to_string(pu.direction.y);
            y_dir.resize(4);
            auto player_dir = x_dir + ", " + y_dir;
            m_termboxPlayerTable.setCell(player_dir, curr_row, curr_col++);

            // update joinable column
            auto joinable = std::to_string(pu.joinable);
            m_termboxPlayerTable.setCell(joinable, curr_row, curr_col);

            curr_row++;
        }
    }
    tb_present();
}
