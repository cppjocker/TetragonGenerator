#include "tetragongenerator.h"
#include "convextetragon.h"
#include "noconvextetragon.h"

#include "geometricutils.h"

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <climits>

namespace{
    const int EBS = 0.1;
}

TetragonGenerator::TetragonGenerator()
{
    srand(time(NULL));
}

Tetragon* TetragonGenerator::getNext(ModeTetragon mode){
    const int rand_max = 2 << 10;
    //[0, 1]
    float x1 = static_cast<float>(rand() % rand_max) / (rand_max - 1);
    float y1 = static_cast<float>(rand() % rand_max) / (rand_max - 1);

    float x2 = 0.f;
    float y2 = 0.f;
    do{
            x2 = static_cast<float>(rand() % rand_max) / (rand_max - 1);
            y2 = static_cast<float>(rand() % rand_max) / (rand_max - 1);
    }while(fabs(x2 - x1) * fabs(y2 - y1) < EBS);

    float x3 = 0.f;
    float y3 = 0.f;

    do{
        x3 = static_cast<float>(rand() % rand_max) / (rand_max - 1);
        y3 = static_cast<float>(rand() % rand_max) / (rand_max - 1);
    }while(!GeometricUtils::test3PointsNotOn1Line(x1, y1, x2, y2, x3, y3, EBS));

    float x4 = 0.f;
    float y4 = 0.f;

    bool availablePoint = true;

    do{
        availablePoint = true;
        x4 = static_cast<float>(rand() % rand_max) / (rand_max - 1);
        y4 = static_cast<float>(rand() % rand_max) / (rand_max - 1);
        availablePoint = availablePoint && GeometricUtils::test3PointsNotOn1Line(x1, y1, x2, y2, x4, y4, EBS);
        availablePoint = availablePoint && GeometricUtils::test3PointsNotOn1Line(x1, y1, x3, y3, x4, y4, EBS);
        availablePoint = availablePoint && GeometricUtils::test3PointsNotOn1Line(x3, y3, x2, y2, x4, y4, EBS);
        bool intersect = GeometricUtils::lineInterscets(x1, y1, x2, y2, x3, y3, x4, y4);
        if(mode == ModeTetragon_Convex){
            availablePoint = availablePoint && intersect;
        }
        else{            
            availablePoint = availablePoint && !intersect;
            availablePoint = availablePoint &&  !GeometricUtils::lineInterscets(x1, y1, x3, y3, x2, y2, x4, y4);
            availablePoint = availablePoint &&  !GeometricUtils::lineInterscets(x1, y1, x2, y2, x4, y4, x3, y3);
        }
    }while(!availablePoint);

    if(mode == ModeTetragon_Convex){
        return new ConvexTetragon(QPointF(x1, y1) , QPointF(x2, y2), QPointF(x3, y3), QPointF(x4, y4) );
    }
    else{
        return new NoConvexTetragon(QPointF(x1, y1) , QPointF(x2, y2), QPointF(x3, y3), QPointF(x4, y4) );
    }
}



