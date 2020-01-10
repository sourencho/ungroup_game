#include "TemporalMetric.hpp"

#include <numeric>

TemporalMetric::TemporalMetric(size_t size, sf::Time time_granularity) :
    m_counts(size), m_timeGranularity(time_granularity) {
}

float TemporalMetric::getRate(sf::Time interval) {
    sf::Time total_time = static_cast<float>(m_counts.size()) * m_timeGranularity;
    if (total_time.asSeconds() == 0 || interval.asSeconds() == 0) {
        return 0;
    }
    std::vector<int> all_counts = getAllCounts();
    int total_count = std::accumulate(all_counts.begin(), all_counts.end(), 0);
    float count_per_second = static_cast<float>(total_count) / total_time.asSeconds();
    float rate = count_per_second / interval.asSeconds();
    return rate;
}

float TemporalMetric::getAverage() {
    std::vector<int> all_counts = getAllCounts();
    int total_count = std::accumulate(all_counts.begin(), all_counts.end(), 0);
    if (all_counts.size() == 0) {
        return 0;
    }
    return static_cast<float>(total_count) / all_counts.size();
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
        m_counts.push_back(m_currentCounts);
        m_currentCounts.clear();
    }
}

std::vector<int> TemporalMetric::getAllCounts() {
    return std::accumulate(m_counts.begin(), m_counts.end(), std::vector<int>(),
                           [](std::vector<int> a, std::vector<int> b) {
                               a.insert(a.end(), b.begin(), b.end());
                               return a;
                           });
}