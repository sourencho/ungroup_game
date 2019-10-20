#ifndef IdFactory_hpp
#define IdFactory_hpp

#include <mutex>
#include <unordered_map>

static const uint32_t OFFSET_VALUE = UINT32_MAX / UINT16_MAX;

class IdFactory {
  public:
    static IdFactory& getInstance() {
        static IdFactory instance;
        return instance;
    }

    // Let's make sure we don't accidentally get copies of the singleton.
    IdFactory(IdFactory const&) = delete;
    void operator=(IdFactory const&) = delete;

    uint32_t getNextId(uint16_t type);
    size_t getIndex(uint32_t id);
    uint16_t getType(uint32_t id);
    void reset();

  private:
    IdFactory() {}

    static std::mutex mTypeToIndex_lock;
    static std::unordered_map<uint16_t, size_t> mTypeToIndex_t;
};

#endif /* IdFactory_hpp */
