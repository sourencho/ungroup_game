/**
 * Keeps track of a metric temporally in order to produce it's rate over an interval of time.
 *
 * Counts are stored in a specified number of slots each representing an equal amount of time.
 * Counts are accumulated in a slot until the interval of time has passed, at which point counts
 * start accumilating in a new slot. There are a limited number of slots and each time a new slot is
 * used, the oldest is discarded.
 *
 * This class currenly only supports int counts.
 */

#include <vector>

#include <SFML/System.hpp>
#include <boost/circular_buffer.hpp>

#ifndef TemporalMetric_hpp
#define TemporalMetric_hpp

class TemporalMetric {
  public:
    /**
     * @size: Number of slots of time to store at once.
     * @time_granularity: The amount of time each slot represents.
     */
    explicit TemporalMetric(size_t size, sf::Time time_granularity);
    ~TemporalMetric(){};
    TemporalMetric(const TemporalMetric& temp_obj) = delete;
    TemporalMetric& operator=(const TemporalMetric& temp_obj) = delete;

    /**
     * If enough time has elapsed then updates slot currently accumulating counts.
     * Should be called as often as possible.
     */
    void update();

    /**
     * Push a count of one.
     */
    void pushCount();

    /**
     * Push a count of @n.
     */
    void pushCount(int n);

    /**
     * Get rate of metric per @interval amount of time
     */
    float getRate(sf::Time interval);

    /**
     * Get average value of metric over the entire time span tracked.
     */
    float getAverage();

    /**
     * Updates the slot currently accumilating the metric.
     */
    void stepSlot();

  private:
    /**
     * Returns the count and sum of all the counts.
     */
    std::pair<size_t, int> accumulateCounts();

    sf::Clock m_clock;
    boost::circular_buffer<std::vector<int>> m_countSlots;
    sf::Time m_timeGranularity;
    std::vector<int> m_currentCounts;
};

#endif /* TemporalMetric_hpp */
