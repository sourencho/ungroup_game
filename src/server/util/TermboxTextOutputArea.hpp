#ifndef termbox_text_output_area_hpp
#define termbox_text_output_area_hpp

#include "../../../extern/termbox/src/termbox.h"

#include <iostream>
#include <string>
#include <vector>

#include <boost/circular_buffer.hpp>

class TermboxTextOutputArea {
  public:
    explicit TermboxTextOutputArea(uint32_t max_height);
    ~TermboxTextOutputArea(){};
    TermboxTextOutputArea(const TermboxTextOutputArea& temp_obj) = delete;
    TermboxTextOutputArea& operator=(const TermboxTextOutputArea& temp_obj) = delete;

    void addLine(std::string line);
    void addTitle(const std::string& title);
    void setLines();
    void setOrigin(uint32_t x, uint32_t y);
    void setMaxHeight(uint32_t max_height);

  private:
    uint32_t setStrAtCoord(std::string str, uint32_t x, uint32_t y, uint16_t fg);
    uint32_t m_origin_x = 0;
    uint32_t m_origin_y = 0;
    boost::circular_buffer<std::string> m_lines;
    std::string m_title;
};

#endif /* termbox_text_output_area_hpp */
