#include "geometricutils.h"

#include <cmath>

bool GeometricUtils::lineInterscets(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4){
    // Ax + By + C A = (y1 - y2), B = (x2 - x1), C = x1y2 - x2y1
    float A1 = y1 - y3;
    float B1 = x3 - x1;
    float C1 = x1 * y3 - x3 * y1;

    float A2 = y2 - y4;
    float B2 = x4 - x2;
    float C2 = x2 * y4 - x4 * y2;

    float xIntersect = (B1 * C2 - B2 * C1) / (A1 * B2 - A2 * B1);

    bool intersect = true;
    double t1 = (xIntersect - x1) / (x3 - x1);
    intersect  = intersect &&  (0. <= t1 && t1 <= 1.);
    double t2 = (xIntersect - x2) / (x4 - x2);
    intersect  = intersect &&  (0. <= t2 && t2 <= 1.);
    return intersect;
}

QPointF GeometricUtils::pointInterscets(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4){
    // Ax + By + C A = (y1 - y2), B = (x2 - x1), C = x1y2 - x2y1
    float A1 = y1 - y3;
    float B1 = x3 - x1;
    float C1 = x1 * y3 - x3 * y1;

    float A2 = y2 - y4;
    float B2 = x4 - x2;
    float C2 = x2 * y4 - x4 * y2;

    float xIntersect = (B1 * C2 - B2 * C1) / (A1 * B2 - A2 * B1);
    float yIntersect =  (C1 * A2 - C2 * A1) / (A1 * B2 - A2 * B1);
    return QPointF(xIntersect, yIntersect);
}


bool GeometricUtils::test3PointsNotOn1Line(float x1, float y1, float x2, float y2, float x3, float y3, double ebs){
    // multiple vector as square parallelogram on vectors v1v2 and v1v3
    return (fabs((x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1)) > ebs);
}
