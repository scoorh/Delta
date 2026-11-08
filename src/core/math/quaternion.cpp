#include <core/types.h>


namespace math::quaternion
{
    void from_euler(quaternion_t *o, const f32 pitch, const f32 yaw, const f32 roll)
    {
        const f32 cx = math::cos(pitch * 0.5f);
        const f32 sx = math::sin(pitch * 0.5f);
        const f32 cy = math::cos(yaw * 0.5f);
        const f32 sy = math::sin(yaw * 0.5f);
        const f32 cz = math::cos(roll * 0.5f);
        const f32 sz = math::sin(roll * 0.5f);
        o->w = cx * cy * cz + sx * sy * sz;
        o->x = sx * cy * cz - cx * sy * sz;
        o->y = cx * sy * cz + sx * cy * sz;
        o->z = cx * cy * sz - sx * sy * cz;
    }
}
