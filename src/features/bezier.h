#ifndef BEZIER_H
#define BEZIER_H

#include <glm/glm.hpp>
#include <vector>

struct BezierPoint {
    float y; // height
    float z; // forward lean
};


class Bezier {

public:
    Bezier();

    // functions
    BezierPoint bezierBasis(float t);
    void resetCurve(float startY);

private:
    // control points
    BezierPoint p0, p1, p2, p3;
};

#endif // BEZIER_H
