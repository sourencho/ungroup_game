#ifndef ThreadSafeVector_hpp
#define ThreadSafeVector_hpp

#include <mutex>
#include <vector>
#include <algorithm>

template <class T>
class ThreadSafeVector {
    std::mutex write_lock;

    public:
        void lock() {
            write_lock.lock();
        }
        void unlock() {
            write_lock.unlock();
        }
        std::vector<T> copy() {
            std::vector<T> vector_copy;
            lock();
            std::copy(mVector.begin(), mVector.end(), std::back_inserter(vector_copy));
            unlock();
            return vector_copy;
        }
        void clear() {
            lock();
            mVector.clear();
            unlock();
        }
        void add(T element) {
            lock();
            mVector.push_back(element);
            unlock();
        }
        std::vector<T> forceCopy() {
            std::vector<T> vector_copy;
            std::copy(mVector.begin(), mVector.end(), std::back_inserter(vector_copy));
            return vector_copy;
        }
        void forceClear() {
            mVector.clear();
        }

    private:
        std::vector<T> mVector;
};

#endif /* ThreadSafeVector_hpp */
