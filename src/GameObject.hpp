#ifndef GameObject_hpp
#define GameObject_hpp

class GameObject {
    public:
        void setActive(bool is_active) {
            mIsActive = is_active;
        }

        bool isActive() const {
            return mIsActive;
        }

    protected:
        bool mIsActive = false;
};

#endif /* GameObject_hpp */
