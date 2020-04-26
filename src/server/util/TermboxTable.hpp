#ifndef termbox_table_hpp
#define termbox_table_hpp

#include "../../../extern/termbox/src/termbox.h"

#include <string>
#include <vector>

class TermboxTable {
    const std::string DELIMETER = "  |  ";
    const uint32_t MIN_COL_WIDTH = 5;

  public:
    void addColumn(std::string name);
    void setCell(const std::string& cell_val, uint32_t row, uint32_t col);
    void setOrigin(uint32_t x, uint32_t y);
    void setColHeaders();

  private:
    uint32_t setStrAtCoord(std::string str, uint32_t x, uint32_t y);

    std::vector<std::string> m_columnNames;
    bool m_columnsInitialized = false;
    uint32_t m_origin_x = 0;
    uint32_t m_origin_y = 0;
};

#endif /* termbox_table_hpp */
