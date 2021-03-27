#include "utils.h"

namespace render {
namespace math {

    Vector3 cross(const Vector3& a, const Vector3& b)
    {
        return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }

    float length(const Vector3& a)
    {
        return static_cast<float>(sqrt(a.x * a.x + a.y * a.y + a.z * a.z));
    }

    Vector3 normalize(const Vector3& a)
    {
        const auto l = length(a);
        return Vector3(a.x / l, a.y / l, a.z / l);
    }


}
}