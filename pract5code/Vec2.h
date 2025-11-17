#ifndef VEC2_H
#define VEC2_H

class Vec2 {
public:
    double x;
    double y;

    Vec2(double x_ = 0.0, double y_ = 0.0);

    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    Vec2 operator*(double s) const;
    Vec2& operator+=(const Vec2& other);

    double dot(const Vec2& other) const;
    double length() const;
    Vec2 normalized() const;
};

#endif // VEC2_H

