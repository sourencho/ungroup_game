#ifndef IdController_hpp
#define IdController_hpp

#include <unordered_map>
#include <mutex>


static const uint32_t OFFSET_VALUE = UINT32_MAX/UINT16_MAX;

class IdController {
 public:
    static IdController& getInstance() {
        static IdController instance;
        return instance;
    }

    // Let's make sure we don't accidentally get copies of the singleton.
    IdController(IdController const&) = delete;
    void operator=(IdController const&) = delete;

    uint32_t getNextId(uint16_t type);
    size_t getIndex(uint32_t id);
    uint16_t getType(uint32_t id);

 private:
    IdController() {}

    static std::mutex mTypeToIndex_lock;
    static std::unordered_map<uint16_t, size_t> mTypeToIndex_t;
};

#endif /* IdController_hpp */
