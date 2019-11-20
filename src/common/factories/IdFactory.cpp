#include "IdFactory.hpp"

#include <iostream>

uint32_t IdFactory::getNextId(uint16_t type) {
    std::lock_guard<std::mutex> m_typeToIndex_guard(m_typeToIndex_lock);

    if (m_typeToIndex_t.count(type) == 0) {
        m_typeToIndex_t[type] = 0;
    }

    size_t index = m_typeToIndex_t[type];
    size_t next_index = index + 1;

    if (next_index >= OFFSET_VALUE) {
        throw std::runtime_error("Id index surpassed max value.");
    }

    m_typeToIndex_t[type] = next_index;

    return (type * OFFSET_VALUE) + index;
}

size_t IdFactory::getIndex(uint32_t id) {
    return id % OFFSET_VALUE;
}

uint16_t IdFactory::getType(uint32_t id) {
    return id / OFFSET_VALUE;
}

void IdFactory::reset() {
    std::lock_guard<std::mutex> m_typeToIndex_guard(m_typeToIndex_lock);
    m_typeToIndex_t.clear();
}

std::mutex IdFactory::m_typeToIndex_lock;
std::unordered_map<uint16_t, size_t> IdFactory::m_typeToIndex_t;
