#include "TemporalMetric.hpp"

#include <numeric>

TemporalMetric::TemporalMetric(size_t size, sf::Time time_granularity) :
    m_counts(size), m_timeGranularity(time_granularity) {
}

float TemporalMetric::getRate(sf::Time interval) {
    sf::Time total_time = static_cast<float>(m_counts.size()) * m_timeGranularity;
    uint total_count = std::accumulate(m_counts.begin(), m_counts.end(), 0);
    float count_per_second = static_cast<float>(total_count) / total_time.asSeconds();
    float rate = count_per_second / interval.asSeconds();
    return rate;
}

void TemporalMetric::incrementCount() {
    m_currentCount += 1;
}

void TemporalMetric::incrementCount(uint n) {
    m_currentCount += n;
}

void TemporalMetric::update() {
    if (m_clock.getElapsedTime() > m_timeGranularity) {
        m_clock.restart();
        m_counts.push_back(m_currentCount);
        m_currentCount = 0;
    }
}