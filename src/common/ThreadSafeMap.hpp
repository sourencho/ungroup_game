#ifndef ThreadSafeMap_hpp
#define ThreadSafeMap_hpp

#include <unordered_map>
#include <mutex>

template <class Key, class Value>
class ThreadSafeMap {
    std::mutex write_lock;

    public:
        void erase(Key key) {
            write_lock.lock();
            mMap.erase(key);
            write_lock.unlock();
        }
        void set(Key key, Value value) {
            write_lock.lock();
            mMap[key] = value;
            write_lock.unlock();
        }
        Value get(Key k) {
            Value value;
            write_lock.lock();
            value = mMap[k];
            write_lock.unlock();
            return value;
        }
        std::unordered_map<Key, Value> forceCopy() {
            std::unordered_map<Key, Value> map_copy;
            map_copy.insert(mMap.begin(), mMap.end());
            return map_copy;
        }
        void lock() {
            write_lock.lock();
        }
        void unlock() {
            write_lock.unlock();
        }

    private:
        std::unordered_map<Key, Value> mMap;
};

#endif /* ThreadSafeMap_hpp */
