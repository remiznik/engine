#pragma once

namespace render {
namespace math {

    struct Vector2
    {
        float x;
        float y;

        Vector2(float _x, float _y) : x(_x), y(_y) {}
        Vector2() = default;
    };

    struct Vector3
    {
        float x;
        float y;
        float z;

        Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
        Vector3() = default;
    };

    struct Vector4
    {
        float x{ 0 };
        float y{ 0 };
        float z{ 0 };
        float w{ 0 };

        Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
        Vector4() = default;
    };

    struct Vertex
    {
        Vector3 pos;
        Vector4 color;
    };

}
}