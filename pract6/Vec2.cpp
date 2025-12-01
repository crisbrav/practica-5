#include "Vec2.h"
#include <cmath>

Vec2::Vec2(double x_, double y_) : x(x_), y(y_) {}

Vec2 Vec2::operator+(const Vec2& other) const {
    return Vec2(x + other.x, y + other.y);
}

Vec2 Vec2::operator-(const Vec2& other) const {
    return Vec2(x - other.x, y - other.y);
}

Vec2 Vec2::operator*(double s) const {
    return Vec2(x * s, y * s);
}

Vec2& Vec2::operator+=(const Vec2& other) {
    x += other.x;
    y += other.y;
    return *this;
}

double Vec2::dot(const Vec2& other) const {
    return x * other.x + y * other.y;
}

double Vec2::length() const {
    return std::sqrt(x * x + y * y);
}

Vec2 Vec2::normalized() const {
    double len = length();
    if (len == 0.0) return Vec2(0.0, 0.0);
    return Vec2(x / len, y / len);
}
