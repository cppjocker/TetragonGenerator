#include "noconvextetragon.h"

#include "geometricutils.h"

#include <QVector>

#include <cmath>

NoConvexTetragon::NoConvexTetragon(const QPointF _p0, QPointF _p1, QPointF _p2, QPointF _p3)
    :Tetragon(_p0, _p1, _p2, _p3){
}

double NoConvexTetragon::getU(double x, double y, bool* ok)const{
    double u = (a1 * x + a2 * y + a3) / (d1 * x + d2 * y + d3);
    u = qAbs(u);
    while(u > 1.){
        int bound = u;
        u = bound - (u - bound);
    }
    if(ok){
        *ok = true;
    }
    return u;
}
double NoConvexTetragon::getV(double x, double y, bool* ok)const {
    double v = (b1 * x + b2 * y + b3) / (d1 * x + d2 * y + d3);
    v = qAbs(v);
    while(v > 1.){
        int bound = v;
        v = bound - (v - bound);
    }
    if(ok){
        *ok = true;
    }
    return v;
}

