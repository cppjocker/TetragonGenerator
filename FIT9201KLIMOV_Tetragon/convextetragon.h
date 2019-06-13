#ifndef CONVEX_TETRAGON_H
#define CONVEX_TETRAGON_H

#include "tetragon.h"

class ConvexTetragon : public Tetragon{
public:
    ConvexTetragon(){}
    ConvexTetragon(const QPointF p0, QPointF p1, QPointF p2, QPointF p3);

    virtual double getU(double x, double y, bool* ok = 0)const;
    virtual double getV(double x, double y, bool* ok = 0)const ;
};

#endif // TETRAGON_H
