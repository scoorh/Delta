#include <core/types.h>


namespace math::vector4
{
    void zero(vector4_t *o)
    {
        o->sse = _mm_setzero_ps();
    }


    f32 dot(vector4_t *a, vector4_t *b)
    {
        f32 result;
        _mm_store_ss(&result, _mm_dp_ps(a->sse, b->sse, 0xFF));
        return result;
    }


    void cross(vector4_t *o, vector4_t *a, vector4_t *b)
    {
        const auto a_yzx = _mm_shuffle_ps(a->sse, a->sse, _MM_SHUFFLE(3, 0, 2, 1));
        const auto b_yzx = _mm_shuffle_ps(b->sse, b->sse, _MM_SHUFFLE(3, 0, 2, 1));
        __m128 c = _mm_sub_ps(_mm_mul_ps(a->sse, b_yzx), _mm_mul_ps(a_yzx, b->sse));
        __m128 r = _mm_shuffle_ps(c, c, _MM_SHUFFLE(3, 0, 2, 1));
        o->sse = _mm_blend_ps(r, _mm_setzero_ps(), 0x8);
    }


    void normalize(vector4_t *o, vector4_t *a)
    {
        o->sse = _mm_div_ps(a->sse, _mm_sqrt_ps(_mm_dp_ps(a->sse, a->sse, 0xFF)));
    }


    void add(vector4_t *o, const vector4_t *a, const vector4_t *b)
    {
        o->sse = _mm_add_ps(a->sse, b->sse);
    }


    void add(vector4_t *o, const vector4_t *a, const f32 v)
    {
        o->sse = _mm_add_ps(a->sse, _mm_set1_ps(v));
    }


    void subtract(vector4_t *o, const vector4_t *a, const vector4_t *b)
    {
        o->sse = _mm_sub_ps(a->sse, b->sse);
    }


    void subtract(vector4_t *o, const vector4_t *a, const f32 v)
    {
        o->sse = _mm_sub_ps(a->sse, _mm_set1_ps(v));
    }


    void multiply(vector4_t *o, const vector4_t *a, const vector4_t *b)
    {
        o->sse = _mm_mul_ps(a->sse, b->sse);
    }


    void multiply(vector4_t *o, const vector4_t *a, const f32 v)
    {
        o->sse = _mm_mul_ps(a->sse, _mm_set1_ps(v));
    }
}
