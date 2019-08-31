#ifndef ThreadSafeData_hpp
#define ThreadSafeData_hpp

#include <mutex>

template <class T>
class ThreadSafeData {
    std::mutex write_lock;

    public:
        void lock() {
            write_lock.lock();
        }
        void unlock() {
            write_lock.unlock();
        }
        void set(T data) {
            lock();
            mData = data;
            unlock();
        }
        T get() {
            T data_copy;
            lock();
            data_copy = mData;
            unlock();
            return data_copy;
        }

    private:
        T mData;
};

#endif /* ThreadSafeData_hpp */
