/* 
Code copied and modified from
https://raytracing.github.io/books/RayTracingInOneWeekend.html#thevec3class/colorutilityfunctions
*/

#ifndef VEC3_H
#define VEC3_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

class vec3 {
public:
    // Public members for direct access
    double x, y, z;

    // Constructors
    vec3() : x(0), y(0), z(0) {}
    vec3(double x, double y, double z) : x(x), y(y), z(z) {}

    // Negation operator
    vec3 operator-() const { return vec3(-x, -y, -z); }

    // Compound assignment operators
    vec3& operator+=(const vec3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    vec3& operator*=(double t) {
        x *= t;
        y *= t;
        z *= t;
        return *this;
    }

    vec3& operator/=(double t) {
        return *this *= 1 / t;
    }

    // Utility functions
    double length() const {
        return std::sqrt(length_squared());
    }

    double length_squared() const {
        return x * x + y * y + z * z;
    }

    sf::Vector2f translate2D(){
        return sf::Vector2f(x, y);
    }
};

// Aliases for clarity in geometric code
using Point3 = vec3;

// Vector utility functions

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.x << ' ' << v.y << ' ' << v.z;
}

inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}

inline vec3 operator*(double t, const vec3& v) {
    return vec3(t * v.x, t * v.y, t * v.z);
}

inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

inline vec3 operator/(const vec3& v, double t) {
    return (1 / t) * v;
}

inline double dot(const vec3& u, const vec3& v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.y * v.z - u.z * v.y,
                u.z * v.x - u.x * v.z,
                u.x * v.y - u.y * v.x);
}

inline vec3 unit_vector(const vec3& v) {
    return v / v.length();
}

#endif