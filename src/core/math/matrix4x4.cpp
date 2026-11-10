#include <core/types.h>


namespace math::matrix4x4
{
    void zero(matrix4x4_t *o)
    {
        const __m256 zero = _mm256_setzero_ps();
        _mm256_storeu_ps(&o->array[0], zero);
        _mm256_storeu_ps(&o->array[8], zero);
    }


    void identity(matrix4x4_t *o)
    {
        zero(o);
        o->_00 = 1.0f;
        o->_11 = 1.0f;
        o->_22 = 1.0f;
        o->_33 = 1.0f;
    }


    void translation(matrix4x4_t *o, const f32 x, const f32 y, const f32 z)
    {
        o->columns[3].x = x;
        o->columns[3].y = y;
        o->columns[3].z = z;
    }


    void scaling(matrix4x4_t *o, const f32 uniform_s)
    {
        assert(uniform_s > 0.0f);
        o->_00 = uniform_s;
        o->_11 = uniform_s;
        o->_22 = uniform_s;
    }


    void rotation(matrix4x4_t *o, const quaternion_t *q)
    {
        const f32 x2 = q->x + q->x;
        const f32 y2 = q->y + q->y;
        const f32 z2 = q->z + q->z;
        const f32 xx = q->x * x2;
        const f32 yy = q->y * y2;
        const f32 zz = q->z * z2;
        const f32 xy = q->x * y2;
        const f32 xz = q->x * z2;
        const f32 yz = q->y * z2;
        const f32 wx = q->w * x2;
        const f32 wy = q->w * y2;
        const f32 wz = q->w * z2;
        o->columns[0].x = 1.0f - (yy + zz);
        o->columns[0].y = xy + wz;
        o->columns[0].z = xz - wy;
        o->columns[1].x = xy - wz;
        o->columns[1].y = 1.0f - (xx + zz);
        o->columns[1].z = yz + wx;
        o->columns[2].x = xz + wy;
        o->columns[2].y = yz - wx;
        o->columns[2].z = 1.0f - (xx + yy);
    }


    void world_transform(matrix4x4_t *o, const f32 tx, const f32 ty, const f32 tz, const f32 uniform_s, const quaternion_t *q)
    {
        assert(uniform_s > 0.0f);
        rotation(o, q);
        o->columns[0].x *= uniform_s;
        o->columns[0].y *= uniform_s;
        o->columns[0].z *= uniform_s;
        o->columns[1].x *= uniform_s;
        o->columns[1].y *= uniform_s;
        o->columns[1].z *= uniform_s;
        o->columns[2].x *= uniform_s;
        o->columns[2].y *= uniform_s;
        o->columns[2].z *= uniform_s;
        /*
        o->columns[0].x *= sx;
        o->columns[0].y *= sx;
        o->columns[0].z *= sx;
        o->columns[1].x *= sy;
        o->columns[1].y *= sy;
        o->columns[1].z *= sy;
        o->columns[2].x *= sz;
        o->columns[2].y *= sz;
        o->columns[2].z *= sz;
        */
        o->columns[3].x = tx;
        o->columns[3].y = ty;
        o->columns[3].z = tz;
    }


    void perspective(matrix4x4_t *o, const f32 aspect_ratio, const f32 field_of_view, const f32 near_z, const f32 far_z)
    {
        const float tanhfov = math::tan(field_of_view * 0.5f);
        const float f = 1.0f / tanhfov;
        const float k = far_z * (1.0f / (far_z - near_z));
        o->array[0] = f / aspect_ratio;
        o->array[5] = -f;
        o->array[10] = k;
        o->array[11] = 1.0f;
        o->array[14] = -near_z * k;
        o->array[15] = 0.0f;
    }


    void look_at(matrix4x4_t *o, const f32 ex, const f32 ey, const f32 ez, const f32 tx, const f32 ty, const f32 tz, const f32 ux, const f32 uy, const f32 uz)
    {
        vector4_t f;
        vector4_t r;
        vector4_t u;

        vector4_t tmp = {ex - tx, ey - ty, ez - tz, 0.0f};
        vector4_t tmp2 = {ux, uy, uz, 0.0f};

        vector4::normalize(&f, &tmp);
        vector4::cross(&tmp, &tmp2, &f);
        vector4::normalize(&r, &tmp);
        vector4::cross(&u, &f, &r);

        o->array[0] = r.x;
        o->array[1] = u.x;
        o->array[2] = -f.x;
        o->array[3] = 0.0f;
        o->array[4] = r.y;
        o->array[5] = u.y;
        o->array[6] = -f.y;
        o->array[7] = 0.0f;
        o->array[8] = r.z;
        o->array[9] = u.z;
        o->array[10] = -f.z;
        o->array[11] = 0.0f;
        o->array[12] = -(r.x * ex + r.y * ey + r.z * ez);
        o->array[13] = -(u.x * ex + u.y * ey + u.z * ez);
        o->array[14] = f.x * ex + f.y * ey + f.z * ez;
        o->array[15] = 1.0f;
    }


    void look_at(matrix4x4_t *o, const vector4_t *from, const quaternion_t *q)
    {
        look_at(o, from->x, from->y, from->z, q);
    }


    void look_at(matrix4x4_t *o, const f32 ex, const f32 ey, const f32 ez, const quaternion_t *q)
    {
        const f32 x = -q->x;
        const f32 y = -q->y;
        const f32 z = -q->z;
        const f32 w = q->w;
        const f32 xx = x * x;
        const f32 yy = y * y;
        const f32 zz = z * z;
        const f32 xy = x * y;
        const f32 xz = x * z;
        const f32 yz = y * z;
        const f32 wx = w * x;
        const f32 wy = w * y;
        const f32 wz = w * z;
        o->array[0] = 1.0f - 2.0f * (yy + zz);
        o->array[1] = 2.0f * (xy + wz);
        o->array[2] = 2.0f * (xz - wy);
        o->array[3] = 0.0f;
        o->array[4] = 2.0f * (xy - wz);
        o->array[5] = 1.0f - 2.0f * (xx + zz);
        o->array[6] = 2.0f * (yz + wx);
        o->array[7] = 0.0f;
        o->array[8] = 2.0f * (xz + wy);
        o->array[9] = 2.0f * (yz - wx);
        o->array[10] = 1.0f - 2.0f * (xx + yy);
        o->array[11] = 0.0f;
        o->array[12] = -(o->array[0] * ex + o->array[4] * ey + o->array[8] * ez);
        o->array[13] = -(o->array[1] * ex + o->array[5] * ey + o->array[9] * ez);
        o->array[14] = (o->array[2] * ex + o->array[6] * ey + o->array[10] * ez);
        o->array[15] = 1.0f;
    }


    void multiply(matrix4x4_t *o, const matrix4x4_t *a, const matrix4x4_t *b)
    {
        for (u32 c = 0; c < 4; ++c)
        {
            const __m128 r0 = _mm_mul_ps(_mm_set1_ps(b->columns[c].x), a->columns[0].sse);
            const __m128 r1 = _mm_mul_ps(_mm_set1_ps(b->columns[c].y), a->columns[1].sse);
            const __m128 r2 = _mm_mul_ps(_mm_set1_ps(b->columns[c].z), a->columns[2].sse);
            const __m128 r3 = _mm_mul_ps(_mm_set1_ps(b->columns[c].w), a->columns[3].sse);
            o->columns[c].sse = _mm_add_ps(_mm_add_ps(r0, r1), _mm_add_ps(r2, r3));
        }
    }
}
