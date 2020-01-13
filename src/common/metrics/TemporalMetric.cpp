#include "TemporalMetric.hpp"

#include <iostream>
#include <numeric>

TemporalMetric::TemporalMetric(size_t size, sf::Time time_granularity) :
    m_countSlots(size), m_timeGranularity(time_granularity) {
}

float TemporalMetric::getRate(sf::Time interval) {
    sf::Time total_time = static_cast<float>(m_countSlots.size()) * m_timeGranularity;
    if (total_time.asSeconds() == 0 || interval.asSeconds() == 0) {
        return 0;
    }
    size_t total_count;
    int total_sum;
    std::tie(total_count, total_sum) = accumulateCounts();
    float count_per_second = static_cast<float>(total_sum) / total_time.asSeconds();
    float rate = count_per_second / interval.asSeconds();
    return rate;
}

float TemporalMetric::getAverage() {
    size_t total_count;
    int total_sum;
    std::tie(total_count, total_sum) = accumulateCounts();
    if (total_count == 0) {
        return 0;
    }
    return static_cast<float>(total_sum) / total_count;
}

void TemporalMetric::pushCount() {
    m_currentCounts.push_back(1);
}

void TemporalMetric::pushCount(int n) {
    m_currentCounts.push_back(n);
}

void TemporalMetric::update() {
    if (m_clock.getElapsedTime() > m_timeGranularity) {
        m_clock.restart();
        stepSlot();
    }
}

void TemporalMetric::stepSlot() {
    m_countSlots.push_back(m_currentCounts);
    m_currentCounts.clear();
}

std::pair<size_t, int> TemporalMetric::accumulateCounts() {
    size_t total_count = 0;
    int total_sum = 0;
    for (auto& count_slot : m_countSlots) {
        for (auto& count : count_slot) {
            total_count += 1;
            total_sum += count;
        }
    }
    return std::make_pair(total_count, total_sum);
}