#ifndef TETRAGON_H
#define TETRAGON_H

#include <QPointF>
#include <QVector>

enum ModeTetragon{
    ModeTetragon_Convex,
    ModeTetragon_NoConvex
};

class Tetragon{
public:
    Tetragon():p0(QPointF(0, 0.)), p1(QPointF(0, 0.)), p2(QPointF(0, 0.)), p3(QPointF(0, 0.)){init();}
    Tetragon(const QPointF& _p0, const QPointF& _p1, const QPointF& _p2, const QPointF& _p3 ):
        p0(_p0), p1(_p1), p2(_p2), p3(_p3){init();}
    virtual double getU(double x, double y, bool* ok = 0)const = 0;
    virtual double getV(double x, double y, bool* ok = 0)const = 0;

    float getUpMax()const{
        return qMax(p0.y(), qMax(p1.y(), qMax(p2.y(), p3.y())));
    }
    float getDownMin()const{
        return qMin(p0.y(), qMin(p1.y(), qMin(p2.y(), p3.y())));
    }
    float getRightMax()const{
        return qMax(p0.x(), qMax(p1.x(), qMax(p2.x(), p3.x())));
    }
    float getLeftMin()const{
        return qMin(p0.x(), qMin(p1.x(), qMin(p2.x(), p3.x())));
    }
    void init();

    void setTransform();
    void setTransform2();

    QPointF getP0()const{return p0;}
    QPointF getP1()const{return p1;}
    QPointF getP2()const{return p2;}
    QPointF getP3()const{return p3;}
protected:
    QPointF p0;
    QPointF p1;
    QPointF p2;
    QPointF p3;
protected:
    static const int DEGREE_TRANSFORM_2 = 3;
    QVector<double> transform2(double coeff[DEGREE_TRANSFORM_2][DEGREE_TRANSFORM_2 + 1]);
    double findDet(double coeff[DEGREE_TRANSFORM_2][DEGREE_TRANSFORM_2]);
protected:
    double a1, a2, a3;
    double b1, b2, b3;
    double d1, d2, d3;
};

#endif // TETRAGON_H
