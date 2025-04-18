// w_math.h
#ifndef W_MATH_H
#define W_MATH_H

namespace w_math {
    typedef struct{
        float x, y;
    } vec2;

    typedef struct{
        float x, y, z;
    } vec3;

    typedef struct{
        int x, y;
    } ivec2;

    typedef struct{
        int x, y, z;
    } ivec3;

    // typedef struct{
    //     int32_t x, y;
    // } i32vec2;
    //
    // typedef struct{
    //     int32_t x, y, z;
    // } i32vec3;
    //
    // typedef struct{
    //     int64_t x, y;
    // } i64vec2;
    //
    // typedef struct{
    //     int64_t x, y, z;
    // } i64vec3;
    
    //sin
    //cos
    //tan

    float add(float a, float b);
    float sub(float a, float b);
    float mul(float a, float b);
    float div(float a, float b);
}

#endif
