#include "IdController.hpp"

#include <iostream>

uint32_t IdController::getNextId(uint16_t type) {
    std::lock_guard<std::mutex> mTypeToIndex_guard(mTypeToIndex_lock);

    if (mTypeToIndex_t.count(type) == 0) {
        mTypeToIndex_t[type] = 0;
    }

    size_t index = mTypeToIndex_t[type];
    size_t next_index = index + 1;

    if (next_index >= OFFSET_VALUE) {
        throw std::runtime_error("Id index surpassed max value.");
    }

    mTypeToIndex_t[type] = next_index;

    return (type * OFFSET_VALUE) + index;
}

size_t IdController::getIndex(uint32_t id) {
    return id % OFFSET_VALUE;
}

uint16_t IdController::getType(uint32_t id) {
    return id / OFFSET_VALUE;
}

std::mutex IdController::mTypeToIndex_lock;
std::unordered_map<uint16_t, size_t> IdController::mTypeToIndex_t;
