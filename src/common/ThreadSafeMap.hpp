#ifndef ThreadSafeMap_hpp
#define ThreadSafeMap_hpp

#include <unordered_map>
#include <mutex>
#include <iostream>


template <class Key, class Value>
class ThreadSafeMap {
    std::mutex write_lock;

    public:
        void erase(Key key) {
            lock();
            mMap.erase(key);
            unlock();
        }
        void set(Key key, Value value) {
            lock();
            mMap[key] = value;
            unlock();
        }
        void forceSet(Key key, Value value) {
            mMap[key] = value;
        }
        Value get(Key k) {
            Value value;
            lock();
            value = mMap[k];
            unlock();
            return value;
        }
        Value has_key(Key k) {
            lock();
            bool contains_key = mMap.find(k) != mMap.end();
            unlock();
            return contains_key;
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
