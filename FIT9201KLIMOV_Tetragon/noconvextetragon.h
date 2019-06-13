#ifndef NOCONVEXTETRAGON_H
#define NOCONVEXTETRAGON_H

#include "tetragon.h"

class NoConvexTetragon : public Tetragon{
public:
    NoConvexTetragon(){}
    NoConvexTetragon(const QPointF p0, QPointF p1, QPointF p2, QPointF p3);

    virtual double getU(double x, double y, bool* ok = 0)const;
    virtual double getV(double x, double y, bool* ok = 0)const ;
};

#endif // NOCONVEXTETRAGON_H
