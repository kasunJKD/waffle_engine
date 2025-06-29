#ifndef DEFINES_H
#define DEFINES_H

#include <cmath>
struct vec3 {
    float x,y,z;

    vec3(float x_, float y_, float z_) {
        x = x_;
        y = y_;
        z = z_;
    }

    inline float length() const noexcept {
        return std::sqrt(x * x + y*y + z*z);
    }

    inline vec3 normalize() {
        float len = length();
        if (len != 0.0f) {
            return {
                x /= len,
                y /= len,
                z /= len,
            };
        }

        return *this;
    }

    inline vec3 operator-(const vec3 &rhs) const noexcept {
        return {x - rhs.x, y - rhs.y, z - rhs.z};
    }

    inline vec3 operator-=(const vec3 &rhs) noexcept {
        x -= rhs.x; y -= rhs.y; z -= rhs.z;
        return *this;
    }

    inline vec3 cross(const vec3 &rhs) const noexcept {
        return {
            (y * rhs.z) - (z * rhs.y),
            (z * rhs.x) - (x * rhs.z),
            (x * rhs.y) - (y * rhs.x)
        };
    }

};

inline vec3 cross(const vec3& a, const vec3& b) noexcept {
    return a.cross(b);
}

//TODO
struct mat4 {};

struct ivec3 {
    int x, y, z;
};

#endif
