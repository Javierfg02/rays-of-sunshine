#include "bezier.h"
#include "settings.h"

Bezier::Bezier() {
    p0 = {settings.baseHeight, 0.0f};
    p1 = {p0.y + settings.stepHeight, 0.2f};  // up and slightly forward
    p2 = {p0.y + settings.stepHeight * 0.5f, -0.2f};  // less height, backward lean
    p3 = {settings.baseHeight, 0.0f};
}

/**
 * @brief Bezier::bezierBasis calculates cubic bezier basis function using lerp
 * @param t
 * @return Bezier basis
 */
BezierPoint Bezier::bezierBasis(float t) {
    t = fmod(t, 1.f);
    float mt = 1 - t;

    BezierPoint result;
    result.y = mt * mt * mt * p0.y +
               3.0f * mt * mt * t * p1.y +
               3.0f * mt * t * t * p2.y +
               t * t * t * p3.y;

    result.z = mt * mt * mt * p0.z +
               3.0f * mt * mt * t * p1.z +
               3.0f * mt * t * t * p2.z +
               t * t * t * p3.z;

    return result;
}

