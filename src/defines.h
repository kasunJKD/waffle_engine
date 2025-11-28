#ifndef DEFINES_H
#define DEFINES_H

#include <array>
#include <cmath>

struct vec3 {
    float x,y,z;

    vec3() = default;

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
struct mat4 {
    /*  | 0  4  8 12 |
        | 1  5  9 13 |
        | 2  6 10 14 |
        | 3  7 11 15 |   */
    std::array<float, 16> m{};

    /* identity by default */
    constexpr mat4() {
        m[0]=m[5]=m[10]=m[15]=1.f;
    }

    constexpr const float* data() const noexcept { return m.data(); }
    constexpr       float* data()       noexcept { return m.data(); }

    /* look-at factory (right-handed, OpenGL) */
    static mat4 lookAt(const vec3& eye,
                       const vec3& center,
                       const vec3& up0)
    {
        vec3 f = (center - eye).normalize();         // forward
        vec3 s = cross(f, up0).normalize();          // right
        vec3 u = cross(s, f);                        // true up

        mat4 M;
        /* first column (s) */
        M.m[0]  =  s.x;  M.m[1]  =  s.y;  M.m[2]  =  s.z;  M.m[3]  = 0.f;
        /* second column (u) */
        M.m[4]  =  u.x;  M.m[5]  =  u.y;  M.m[6]  =  u.z;  M.m[7]  = 0.f;
        /* third column (−f) */
        M.m[8]  = f.x;  M.m[9]  = f.y;  M.m[10] = f.z;  M.m[11] = 0.f;
        /* translation column */
        M.m[12] = -(s.x*eye.x + s.y*eye.y + s.z*eye.z);
        M.m[13] = -(u.x*eye.x + u.y*eye.y + u.z*eye.z);
        M.m[14] =   -(f.x*eye.x + f.y*eye.y + f.z*eye.z);
        M.m[15] = 1.f;
        return M;
    }

    static mat4 translate(const vec3& t) {
        mat4 result = mat4();

        result.m[12] = t.x;
        result.m[13] = t.y;
        result.m[14] = t.z;

        return result;
    }

    static mat4 ortho(float left, float right, float bottom, float top, float n, float f)
    {
        mat4 M = {};

        M.m[0]  = 2.0f / (right - left);
        M.m[5]  = 2.0f / (top - bottom);
        M.m[10] = -2.0f / (f - n);

        M.m[12] = -(right + left) / (right - left);
        M.m[13] = -(top + bottom) / (top - bottom);
        M.m[14] = -(f + n)  / (f - n);

        M.m[15] = 1.0f;

        return M;
    }
    static mat4 scale(const vec3& s) {
        mat4 M = mat4();     // identity

        M.m[0]  = s.x;       // scale X
        M.m[5]  = s.y;       // scale Y
        M.m[10] = s.z;       // scale Z

        return M;
    }
mat4 operator*(const mat4& b) const {
    mat4 result{};
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            result.m[col*4 + row] =
                m[0*4 + row] * b.m[col*4 + 0] +
                m[1*4 + row] * b.m[col*4 + 1] +
                m[2*4 + row] * b.m[col*4 + 2] +
                m[3*4 + row] * b.m[col*4 + 3];
        }
    }
    return result;
}
};

struct ivec3 {
    int x, y, z;
};

#endif
