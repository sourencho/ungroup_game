#ifndef MineController_hpp
#define MineController_hpp

#include <vector>

#include "../objects/Mine.hpp"
#include "../resources/ResourceStore.hpp"

class MineController {
  public:
    MineController(std::vector<std::shared_ptr<Mine>>& mines);
    ~MineController();
    MineController(const MineController& temp_obj) = delete;
    MineController& operator=(const MineController& temp_obj) = delete;

    uint32_t createMine();
    void draw(sf::RenderTarget& target);
    void update();
    void updatePostPhysics();
    std::shared_ptr<Mine>& getMine(uint32_t mine_id);

  private:
    std::vector<std::shared_ptr<Mine>> mMines;
    size_t nextMineIndex = 0;
};

#endif /* MineController_hpp */