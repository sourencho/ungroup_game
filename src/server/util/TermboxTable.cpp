#include "TermboxTable.hpp"
#include <iostream>

void TermboxTable::addColumn(std::string name) {
    m_columnNames.push_back(name);
}

void TermboxTable::setCell(const std::string& cell_val, uint32_t row, uint32_t col) {
    // can't be ran until setColHeaders has been called since that determines column width
    if (!m_columnsInitialized || col > m_columnNames.size()) {
        std::string error =
            "ERROR: attempt to run setCell without running setColHeaders or invalid col value";
        setStrAtCoord(error, m_origin_x, m_origin_y);
        return;
    }

    uint32_t x_offset = m_origin_x;
    uint32_t y_offset = m_origin_y + row;
    for (uint32_t i = 0; i < m_columnNames.size(); i++) {
        // we've found the correct column
        if (i == col) {
            setStrAtCoord(cell_val, x_offset, y_offset);
        } else {
            x_offset += m_columnNames[i].length() + TermboxTable::DELIMETER.length();
        }
    }
}

void TermboxTable::setOrigin(unsigned int x, unsigned int y) {
    m_origin_x = x;
    m_origin_y = y;
}

uint32_t TermboxTable::setStrAtCoord(std::string str, uint32_t x, uint32_t y) {
    uint32_t i;
    for (i = x; i < str.length() + x; i++) {
        tb_change_cell(i, y, str[i - x], 0, 0);
    }
    return i;
}

void TermboxTable::setColHeaders() {
    uint32_t curr_x = m_origin_x;
    for (auto& col_name : m_columnNames) {
        // add padding to assure that column is at least a minimum width
        // this way is pretty inefficient, but only runs once and N is small.
        while (col_name.length() < TermboxTable::MIN_COL_WIDTH) {
            col_name += " ";
        }
        curr_x = setStrAtCoord(col_name, curr_x, m_origin_y);

        // we don't want to set a delimeter for the last column
        if (&col_name != &m_columnNames.back()) {
            curr_x = setStrAtCoord(TermboxTable::DELIMETER, curr_x, m_origin_y);
        }
    }
    m_columnsInitialized = true;
}
