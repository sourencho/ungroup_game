#ifndef GameObject_hpp
#define GameObject_hpp

#include <iostream>

class GameObject {
  public:
    explicit GameObject(uint32_t id) {
        mId = id;
    }

    void setActive(bool is_active) {
        m_isActive = is_active;
    }

    bool isActive() const {
        return m_isActive;
    }

    int getId() const {
        return mId;
    }

  protected:
    uint32_t mId;
    bool m_isActive = false;
};

#endif /* GameObject_hpp */
