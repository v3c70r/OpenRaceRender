#pragma once

struct SVec2
{
    float x, y;
    SVec2 operator-(const SVec2& other) const
    {
        return SVec2({x - other.x, y - other.y});
    }
    SVec2 operator/(const float& val) const { return SVec2({x / val, y / val}); }
};

struct SAABB
{
    SVec2 upper;
    SVec2 lower;
};


