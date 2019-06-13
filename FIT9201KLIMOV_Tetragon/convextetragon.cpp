#include "convextetragon.h"

#include "geometricutils.h"

#include <cstdio>
#include <cmath>
#include <cassert>

#include <QVector>

ConvexTetragon::ConvexTetragon(const QPointF _p0, QPointF _p1, QPointF _p2, QPointF _p3)
    :Tetragon(_p0, _p1, _p2, _p3){
}

double ConvexTetragon::getU(double x, double y, bool* ok)const{
    double u = (a1 * x + a2 * y + a3) / (d1 * x + d2 * y + d3);
    if(0. > u || u > 1.){
        if(ok){
            *ok = false;
        }
        return u;
    }
    if(ok){
        *ok = true;
    }
    return u;
}
double ConvexTetragon::getV(double x, double y, bool* ok)const {
    double v = (b1 * x + b2 * y + b3) / (d1 * x + d2 * y + d3);
    if(0. > v || v > 1.){
        if(ok){
            *ok = false;
        }
        return v;
    }
    if(ok){
        *ok = true;
    }
    return v;
}

