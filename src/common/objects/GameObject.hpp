#ifndef GameObject_hpp
#define GameObject_hpp

#include <iostream>

class GameObject {
  public:
    explicit GameObject(uint32_t id) { mId = id; }

    void setActive(bool is_active) { m_IsActive = is_active; }

    bool isActive() const { return m_IsActive; }

    int getId() const { return mId; }

  protected:
    uint32_t mId;
    bool m_IsActive = false;
};

#endif /* GameObject_hpp */
