#ifndef TETRAGONGENERATOR_H
#define TETRAGONGENERATOR_H

#include <QPointF>
#include <QVector>

#include "tetragon.h"



class TetragonGenerator
{
public:
    TetragonGenerator();
    Tetragon* getNext(ModeTetragon);
private:

};

#endif // TETRAGONGENERATOR_H
