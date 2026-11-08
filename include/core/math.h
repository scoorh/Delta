#pragma once


// ==================================================
// math constants
// ==================================================
namespace math
{
    constexpr f32 infinity = 1e30f;
    constexpr f32 epsilon = 1.192092896e-07f;

    constexpr f32 pi = 3.14159265358979323846f;
    constexpr f32 two_pi = pi * 2.0f;
    constexpr f32 half_pi = pi / 2.0f;
    constexpr f32 quarter_pi = pi / 4.0f;
    constexpr f32 one_over_pi = 1.0f / pi;
    constexpr f32 one_over_two_pi = 1.0f / two_pi;
    constexpr f32 pi_over_180 = pi / 180.0f;
    constexpr f32 _180_over_pi = 180.0f / pi;

    constexpr f32 sqrt_two = 1.414213562373f;
    constexpr f32 sqrt_three = 1.7320508075688772f;
    constexpr f32 sqrt_one_over_two = 0.7071067811865476f;
    constexpr f32 sqrt_one_over_three = 0.5773502691896257f;
}


// ==================================================
// math types
// ==================================================
namespace math
{
    // ==================================================
    // vector4
    // ==================================================
    union vector4_t
    {
        struct
        {
            f32 x;
            f32 y;
            f32 z;
            f32 w;
        };

        f32 array[4];
        __m128 sse;
    };


    // ==================================================
    // quaternion
    // ==================================================
    union quaternion_t
    {
        struct
        {
            f32 w;
            f32 x;
            f32 y;
            f32 z;
        };

        f32 array[4];
        __m128 sse;
    };


    // ==================================================
    // matrix4x4
    // ==================================================
    union matrix4x4_t
    {
        struct
        {
            f32 _00, _01, _02, _03;
            f32 _10, _11, _12, _13;
            f32 _20, _21, _22, _23;
            f32 _30, _31, _32, _33;
        };

        f32 array[16];
        vector4_t columns[4];
    };
}


// ==================================================
// vector4 functions
// ==================================================
namespace math::quaternion
{
    void from_euler(quaternion_t *o, f32 pitch, f32 yaw, f32 roll);
}


// ==================================================
// vector4 functions
// ==================================================
namespace math::vector4
{
    void zero(vector4_t* o);

    f32 dot(vector4_t *a, vector4_t *b);
    void cross(vector4_t* o, vector4_t *a, vector4_t *b);

    void normalize(vector4_t* o, vector4_t *a);

    void add(vector4_t* o, const vector4_t *a, const vector4_t *b);
    void add(vector4_t* o, const vector4_t *a, f32 v);

    void subtract(vector4_t* o, const vector4_t *a, const vector4_t *b);
    void subtract(vector4_t* o, const vector4_t *a, f32 v);

    void multiply(vector4_t* o, const vector4_t *a, const vector4_t *b);
    void multiply(vector4_t* o, const vector4_t *a, f32 v);
}


// ==================================================
// matrix4x4 functions
// ==================================================
namespace math::matrix4x4
{
    void zero(matrix4x4_t* o);
    void identity(matrix4x4_t* o);

    void translation(matrix4x4_t *o, f32 x, f32 y, f32 z);
    void scaling(matrix4x4_t *o, f32 x, f32 y, f32 z);
    void rotation(matrix4x4_t *o, const quaternion_t *q);
    void world_transform(matrix4x4_t *o, f32 tx, f32 ty, f32 tz, f32 sx, f32 sy, f32 sz, const quaternion_t *q);

    void perspective(matrix4x4_t *o, f32 aspect_ratio, f32 field_of_view, f32 near_z, f32 far_z);

    void look_at(matrix4x4_t *o, f32 ex, f32 ey, f32 ez, f32 tx, f32 ty, f32 tz, f32 ux, f32 uy, f32);
    void look_at(matrix4x4_t *o, const vector4_t *from, const quaternion_t *q);
    void look_at(matrix4x4_t *o, f32 ex, f32 ey, f32 ez, const quaternion_t *q);

    void multiply(matrix4x4_t *o, const matrix4x4_t *a, const matrix4x4_t *b);
}


// ==================================================
// math functions
// ==================================================
namespace math
{
    inline f32 tan(const f32 v)
    {
        return ::tanf(v);
    }


    inline f32 cos(const f32 v)
    {
        return ::cosf(v);
    }


    inline f32 sin(const f32 v)
    {
        return ::sinf(v);
    }


    inline f32 seconds_to_miliseconds(const f32 s)
    {
        return s * 1000.0f;
    }


    inline f32 miliseconds_to_seconds(const f32 ms)
    {
        return ms * 0.001f;
    }


    inline f32 degree_to_radian(const f32 d)
    {
        return d * pi_over_180;
    }


    inline f32 radian_to_degree(const f32 r)
    {
        return r * _180_over_pi;
    }


    inline u32 pack_rgba(const u8 r, const u8 g, const u8 b, const u8 a)
    {
        return ((u32) a << 24) | (u32) b << 16 | (u32) g << 8 | (u32) r;
    }


    template<typename T>
    u32 clamp(T c, const T min, const T max)
    {
        c = c < min ? min : c;
        c = c > max ? max : c;
        return c;
    }
}
