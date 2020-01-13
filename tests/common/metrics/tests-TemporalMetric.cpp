#include <catch.hpp>

#include "../../../src/common/metrics/TemporalMetric.hpp"

TEST_CASE("Average", "[common][metrics][TemporalMetric][getAverage]") {
    TemporalMetric temporal_metric(10, sf::seconds(1));
    temporal_metric.pushCount(1);
    temporal_metric.pushCount(5);
    temporal_metric.stepSlot();
    temporal_metric.pushCount(-1);
    temporal_metric.pushCount(3);
    temporal_metric.stepSlot();

    REQUIRE(temporal_metric.getAverage() == 2);
}

TEST_CASE("Rate", "[common][metrics][TemporalMetric][getRate]") {
    TemporalMetric temporal_metric(10, sf::seconds(1));
    temporal_metric.pushCount(1);
    temporal_metric.pushCount(5);
    temporal_metric.stepSlot();
    temporal_metric.pushCount(-1);
    temporal_metric.pushCount(3);
    temporal_metric.stepSlot();

    REQUIRE(temporal_metric.getRate(sf::seconds(1)) == 4);
    REQUIRE(temporal_metric.getRate(sf::seconds(2)) == 2);
    REQUIRE(temporal_metric.getRate(sf::seconds(0.5)) == 8);
}