#ifndef GameObject_hpp
#define GameObject_hpp

#include <iostream>

class GameObject {
    public:
        explicit GameObject(uint32_t id) {
            mId = id;
        }

        void setActive(bool is_active) {
            mIsActive = is_active;
        }

        bool isActive() const {
            return mIsActive;
        }

        int getId() const {
            return mId;
        }

    protected:
        uint32_t mId;
        bool mIsActive = false;
};

#endif /* GameObject_hpp */
