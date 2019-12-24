#include "VectorUtil.hpp"

#include <cmath>
#include <iostream>

float VectorUtil::length(const sf::Vector2f& source) {
    return sqrt((source.x * source.x) + (source.y * source.y));
}

sf::Vector2f VectorUtil::normalize(const sf::Vector2f& source) {
    float l = length(source);
    if (l != 0)
        return sf::Vector2f(source.x / l, source.y / l);
    else
        return source;
}

float VectorUtil::distance(const sf::Vector2f& point_a, const sf::Vector2f& point_b) {
    return sqrt(pow((point_b.x - point_a.x), 2.f) + pow((point_b.y - point_a.y), 2.f));
}

sf::Vector2f VectorUtil::getVector(const sf::Vector2f& point_a, const sf::Vector2f& point_b) {
    return sf::Vector2f(point_b.x - point_a.x, point_b.y - point_a.y);
}

sf::Vector2f VectorUtil::getMidpoint(const sf::Vector2f& point_a, const sf::Vector2f& point_b) {
    return sf::Vector2f((point_a.x + point_b.x) / 2.f, (point_a.y + point_b.y) / 2.f);
}

sf::Vector2f VectorUtil::lerp(const sf::Vector2f& a, const sf::Vector2f& b, float t) {
    return (1 - t) * a + t * b;
}

void VectorUtil::print(const sf::Vector2f& v) {
    std::cout << "(" << v.x << "," << v.y << ")" << std::endl;
}

float VectorUtil::dot(const sf::Vector2f& a, const sf::Vector2f& b) {
    return a.x * b.x + a.y * b.y;
}

double VectorUtil::cos_sim(sf::Vector2f A, sf::Vector2f B) {
    double dot = 0.0, denom_a = 0.0, denom_b = 0.0;

    dot += A.x * B.x;
    denom_a += A.x * A.x;
    denom_b += B.x * B.x;

    dot += A.y * B.y;
    denom_a += A.y * A.y;
    denom_b += B.y * B.y;

    if (denom_a == 0 || denom_b == 0) {
        std::cerr << "cosine similarity is not defined whenever one or both "
                  << "input vectors are zero-vectors." << std::endl;
    }

    return dot / (sqrt(denom_a) * sqrt(denom_b));
}

void VectorUtil::clamp(sf::Vector2f& a, float min, float max) {
    a.x = std::min(a.x, max);
    a.x = std::max(a.x, min);
    a.y = std::min(a.y, max);
    a.y = std::max(a.y, min);
}

float VectorUtil::angle(const sf::Vector2f& v) {
    return atan2(v.y, v.x) * 180.f / M_PI;
}

sf::Vector2f VectorUtil::direction(float angle_degrees) {
    float angle_radians = angle_degrees * M_PI / 180.f;
    return sf::Vector2f(std::cos(angle_radians), std::sin(angle_radians));
}