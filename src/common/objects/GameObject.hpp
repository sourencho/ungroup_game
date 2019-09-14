#ifndef GameObject_hpp
#define GameObject_hpp

class GameObject {
    public:
        GameObject(unsigned int id) {
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
        unsigned int mId;
        bool mIsActive = false;
};

#endif /* GameObject_hpp */
