#include "tetragon.h"


#include <cstdio>
#include <cmath>
#include <cassert>

#include <QVector>

void Tetragon::init(){
    a1 = 0.;
    a2 = 0.;
    a3 = 0.;
    b1 = 0.;
    b2 = 0.;
    b3 = 0.;
    d1 = 0.;
    d2 = 0.;
    d3 = 0.;
}

void Tetragon::setTransform(){
    static const int DEGREE = 9;
    static const int NUM_V_EQUATIONS = 4;
    static const int NUM_U_EQUATIONS = 4;

    double x0 = p0.x();
    double x1 = p1.x();
    double x2 = p2.x();
    double x3 = p3.x();
    double y0 = p0.y();
    double y1 = p1.y();
    double y2 = p2.y();
    double y3 = p3.y();

    QVector<double> xTetragon;
    xTetragon << x0 << x1 << x2 << x3;
    QVector<double> yTetragon;
    yTetragon << y0 << y1 << y2 << y3;
    QVector<double> uRectangle;
    uRectangle << 0 << 0 << 1 << 1 ;
    QVector<double> vRectangle;
    vRectangle  << 1 << 0 << 0 <<  1;


    double m[DEGREE ][DEGREE + 1] ;
        for(int i = 0; i < DEGREE ; ++i){
            for(int j = 0; j <DEGREE + 1; ++j){
                m[i][j] = 0.;
            }
        }
        for(int i = 0; i < NUM_U_EQUATIONS  ; ++i){
            // a1, a2, a3
            m[i][0] = xTetragon.at(i);
            m[i][1] = yTetragon.at(i);
            m[i][2] = 1;
            // d1, d2, d3
            m[i][6] = - xTetragon.at(i) * uRectangle.at(i);
            m[i][7] = - yTetragon.at(i) * uRectangle.at(i);
            m[i][8] = - uRectangle.at(i);
        }
        for(int i =  0  ; i <  NUM_V_EQUATIONS; ++i){
            // a1, a2, a3
            m[i + NUM_U_EQUATIONS][3] = xTetragon.at(i);
            m[i + NUM_U_EQUATIONS][4] = yTetragon.at(i);
            m[i + NUM_U_EQUATIONS][5] = 1;
            // d1, d2, d3
            m[i + NUM_U_EQUATIONS][6] = - xTetragon.at(i) * vRectangle.at(i);
            m[i + NUM_U_EQUATIONS][7] = - yTetragon.at(i) * vRectangle.at(i);
            m[i + NUM_U_EQUATIONS][8] = - vRectangle.at(i);
        }
        m[8][8] = 1.0; // d3 = 1;
        m[8][9] = 1.0;

        // gauss
        for(int i = 0; i < DEGREE ; ++i){
            if (fabs(m[i][i]) < 0.000001){
                for(int j = i + 1; j < DEGREE ; ++j){
                    if(fabs(m[j][i]) >= 0.000001){
                        for(int k = i; k < DEGREE + 1; ++k){
                            m[i][k] += m [j][k];
                        }
                        break;
                    }
                }
            }
            for(int j = i + 1; j < DEGREE ; ++j){
                if(fabs(m[j][i]) < 0.000001){
                    continue;
                }
                double coeff = m[j][i] / m[i][i];
                for(int k = i; k < DEGREE + 1; ++k){
                    m[j][k] -= m [i][k] * coeff;
                }
            }
        }

        double det = 1;
        for(int i = 0; i < DEGREE; ++i){
            det *= m[i][i];
        }

        if(fabs(det) < 0.0000000001 ){
            a1  = 0;
            a2  = 0;
            a3  = 0;
            b1  = 0;
            b2  = 0;
            b3  = 0;
            d1  = 0 ;
            d2  = 0 ;
            d3  = 0 ;
            return;
        }

        double results[DEGREE ];
        QVector <double> _coeffs;
        // back
        for(int i = DEGREE - 1; i>= 0 ; --i){
            double nextCoeff = 0.;
            for(int j = DEGREE - 1; j > i; --j){
                nextCoeff -= results[j] * m[i][j];
            }
            nextCoeff += m[i][DEGREE];
            nextCoeff /= m[i][i];
            results[i] = nextCoeff;
            _coeffs.push_front(nextCoeff);
        }
        a1  = _coeffs.at(0);
        a2  = _coeffs.at(1);
        a3  = _coeffs.at(2);
        b1  = _coeffs.at(3);
        b2  = _coeffs.at(4);
        b3  = _coeffs.at(5);
        d1  = _coeffs.at(6) ;
        d2  = _coeffs.at(7) ;
        d3  = _coeffs.at(8) ;
}

void Tetragon::setTransform2(){
    double m[DEGREE_TRANSFORM_2][DEGREE_TRANSFORM_2 + 1];
    m[0][0] = p0.x(); m[0][1] = p0.y(); m[0][2] = 1; m[0][3] = 0;
    m[1][0] = p1.x(); m[1][1] = p1.y(); m[1][2] = 1; m[1][3] = 0;
    m[2][0] = p3.x(); m[2][1] = p3.y(); m[2][2] = 1; m[2][3] = 1;

    QVector<double> a = transform2(m);
    a1 = a.at(0);
    a2 = a.at(1);
    a3 = a.at(2);

    double ua = a1 * p2.x() + a2 * p2.y() + a3;

    m[0][0] = p3.x(); m[0][1] = p3.y(); m[0][2] = 1; m[0][3] = 1;
    m[1][0] = p1.x(); m[1][1] = p1.y(); m[1][2] = 1; m[1][3] = 0;
    m[2][0] = p2.x(); m[2][1] = p2.y(); m[2][2] = 1; m[2][3] = 0;
    QVector<double> b = transform2(m);
    b1 = b.at(0);
    b2 = b.at(1);
    b3 = b.at(2);

    double vb = b1 * p0.x() + b2 * p0.y() + b3;

    m[0][0] = p0.x(); m[0][1] = p0.y(); m[0][2] = 1; m[0][3] = vb;
    m[1][0] = p3.x(); m[1][1] = p3.y(); m[1][2] = 1; m[1][3] = 1;
    m[2][0] = p2.x(); m[2][1] = p2.y(); m[2][2] = 1; m[2][3] = ua;
    QVector<double> d = transform2(m);
    d1 = d.at(0);
    d2 = d.at(1);
    d3 = d.at(2);
}

QVector<double> Tetragon::transform2(double m[DEGREE_TRANSFORM_2][DEGREE_TRANSFORM_2 + 1]){
    // kramer
    double main[DEGREE_TRANSFORM_2][DEGREE_TRANSFORM_2];
    double first[DEGREE_TRANSFORM_2][DEGREE_TRANSFORM_2];
    double second[DEGREE_TRANSFORM_2][DEGREE_TRANSFORM_2];
    double third[DEGREE_TRANSFORM_2][DEGREE_TRANSFORM_2];

    for(int i = 0; i < DEGREE_TRANSFORM_2; ++i){
        for(int j = 0; j < DEGREE_TRANSFORM_2; ++j){
            main[i][j] = first[i][j] = second[i][j] = third[i][j] = m[i][j];
        }
    }
    for(int i = 0; i < DEGREE_TRANSFORM_2; ++i){
            first[i][0] = m[i][DEGREE_TRANSFORM_2 ];
            second[i][1] = m[i][DEGREE_TRANSFORM_2 ];
            third[i][2] = m[i][DEGREE_TRANSFORM_2 ];
    }
    double detMain = findDet(main);

    if(fabs(detMain) < 0.0000000001 ){
            QVector<double> results ;
            results << 0. << 0. << 0;
            return results;
    }
    double detFirst = findDet(first);
    double detSecond = findDet(second);
    double detThird = findDet(third);
    QVector<double> results;
    results << detFirst / detMain << detSecond / detMain << detThird / detMain;
    return results;
}

double Tetragon::findDet(double m[DEGREE_TRANSFORM_2][DEGREE_TRANSFORM_2]){
    const int DEGREE = DEGREE_TRANSFORM_2;
    for(int i = 0; i < DEGREE ; ++i){
        if (fabs(m[i][i]) < 0.000001){
            for(int j = i + 1; j < DEGREE ; ++j){
                if(fabs(m[j][i]) >= 0.000001){
                    for(int k = i; k < DEGREE; ++k){
                        m[i][k] += m[j][k];
                    }
                    break;
                }
            }
        }
        for(int j = i + 1; j < DEGREE ; ++j){
            if(fabs(m[j][i]) < 0.000001){
                continue;
            }
            double coeff = m[j][i] / m[i][i];
            for(int k = i; k < DEGREE ; ++k){
                m[j][k] -= m[i][k] * coeff;
            }
        }
    }

    double det = 1;
    for(int i = 0; i < DEGREE; ++i){
        det *= m[i][i];
    }
    return det;
}
