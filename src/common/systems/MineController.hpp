#ifndef MineController_hpp
#define MineController_hpp

#include <vector>

#include "../objects/Mine.hpp"
#include "ResourceController.hpp"

class MineController {
  public:
    MineController(std::vector<std::shared_ptr<Mine>>& mines, ResourceController& rc);
    ~MineController();
    MineController(const MineController& temp_obj) = delete;
    MineController& operator=(const MineController& temp_obj) = delete;

    uint32_t createMine(sf::Vector2f center_position);
    void update();
    Mine& getMine(uint32_t mine_id);
    std::vector<std::shared_ptr<Mine>>& getMines() {
        return m_mines;
    };
    std::vector<uint32_t> getMineIds();

  private:
    std::vector<std::shared_ptr<Mine>> m_mines;
    size_t nextMineIndex = 0;
    ResourceController& m_resourceController;
};

#endif /* MineController_hpp */
