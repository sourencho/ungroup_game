#ifndef ThreadSafeVector_hpp
#define ThreadSafeVector_hpp

#include <mutex>
#include <vector>
#include <algorithm>

template <class T>
class ThreadSafeVector {
    std::mutex write_lock;

    public:
        std::vector<T> copy() {
            std::vector<T> vector_copy;
            write_lock.lock();
            std::copy(mVector.begin(), mVector.end(), std::back_inserter(vector_copy));
            write_lock.unlock();
            return vector_copy;
        }
        void clear() {
            write_lock.lock();
            mVector.clear();
            write_lock.unlock();
        }
        void push_back(T element) {
            write_lock.lock();
            mVector.push_back(element);
            write_lock.unlock();
        }
        std::vector<T> forceCopy() {
            std::vector<T> vector_copy;
            std::copy(mVector.begin(), mVector.end(), std::back_inserter(vector_copy));
            return vector_copy;
        }
        void lock() {
            write_lock.lock();
        }
        void unlock() {
            write_lock.unlock();
        }

    private:
        std::vector<T> mVector;
};

#endif /* ThreadSafeVector_hpp */
