#include "TermboxTextOutputArea.hpp"

TermboxTextOutputArea::TermboxTextOutputArea(uint32_t max_height) : m_lines(max_height) {
}

void TermboxTextOutputArea::setLines() {
    uint16_t fg = 0;
    uint32_t y = m_origin_y;

    // set the title
    setStrAtCoord(m_title, m_origin_x, y, TB_UNDERLINE);

    uint32_t counter = 1;
    for (auto& line : m_lines) {
        y = m_origin_y + counter;
        // most recent line will be blue
        if (counter == 1) {
            fg = TB_BLUE;
        } else {
            fg = 0;
        }
        setStrAtCoord(line, m_origin_x, y, fg);
        counter++;
    }
}

void TermboxTextOutputArea::addTitle(const std::string& title) {
    m_title = title;
}

void TermboxTextOutputArea::addLine(std::string line) {
    m_lines.push_front(line);
}

void TermboxTextOutputArea::setOrigin(uint32_t x, uint32_t y) {
    m_origin_x = x;
    m_origin_y = y;
}

void TermboxTextOutputArea::setMaxHeight(uint32_t max_height) {
    m_lines.set_capacity(max_height);
}

uint32_t TermboxTextOutputArea::setStrAtCoord(std::string str, uint32_t x, uint32_t y,
                                              uint16_t fg) {
    uint32_t i;
    for (i = x; i < str.length() + x; i++) {
        tb_change_cell(i, y, str[i - x], fg, 0);
    }
    return i;
}
