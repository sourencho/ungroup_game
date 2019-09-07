#ifndef ThreadSafeVector2f_hpp
#define ThreadSafeVector2f_hpp

#include <SFML/Graphics.hpp>
#include <mutex>

class ThreadSafeVector2f {
    std::mutex write_lock;

    public:
        ThreadSafeVector2f(float x, float y):mVector2f(x, y) {}
        void lock() {
            write_lock.lock();
        }
        void unlock() {
            write_lock.unlock();
        }
        void set(sf::Vector2f value) {
            lock();
            mVector2f = value;
            unlock();
        }
        sf::Vector2f get() {
            sf::Vector2f vector_copy;
            lock();
            vector_copy = mVector2f;
            unlock();
            return vector_copy;
        }

    private:
        sf::Vector2f mVector2f;
};

#endif /* ThreadSafeVector2f_hpp */
