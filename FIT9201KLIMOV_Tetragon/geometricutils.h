#ifndef GEOMETRICUTILS_H
#define GEOMETRICUTILS_H

#include <QPointF>

class GeometricUtils
{
public:
    static bool lineInterscets(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
    static bool test3PointsNotOn1Line(float x1, float y1, float x2, float y2, float x3, float y3, double ebs);
    static QPointF pointInterscets(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);

};

#endif // GEOMETRICUTILS_H
