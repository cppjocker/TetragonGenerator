#include "tetragonwindow.h"

#include "geometricutils.h"
#include "convextetragon.h"
#include "noconvextetragon.h"

#include <QPainter>
#include <QTime>
#include <QMouseEvent>

#include <cassert>
#include <cmath>
#include <cstdio>

namespace{
    static const QString TEXTURE_FILE = ":/images/earth3";
    static const int WIDTH_SETTINGS_PANEL = 110;
    static const int HEIGHT_SETTINGS_PANEL = 255;
    static const int PIXEL_BLOCK = 3;
    static const int DEADLINE = 10000;
}

TetragonWindow::TetragonWindow(QWidget *parent) :
    QMainWindow(parent), counter(0), filter(Filter_Simple), gauss(true)
{
    textureImage = QImage(TEXTURE_FILE);

    setupUi(this);    

    calcMipLevels();
    setTetragonArea();
}

void TetragonWindow::setTetragonArea(){
    int availableWidth = this->width() - WIDTH_SETTINGS_PANEL;
    int availableHeight = this->height();
    int min = qMin(availableWidth, availableHeight);

    tetragonArea = QImage(min * 3 / 4, min * 3 / 4, QImage::Format_RGB888);
    tetragonArea.fill(QColor(Qt::gray).rgb());

    littleOriginImage = QImage(min / 4, min / 4, QImage::Format_RGB888);
    littleOriginImage.fill(QColor(Qt::gray).rgb());
}

uint TetragonWindow::safeGetPixel(const QImage& image, int x, int y){
    if(0 <= x && x <= image.width() - 1 && 0 <= y && y <= image.height() - 1){
        return image.pixel(x, y);
    }
    else{
        x = qMax(0, x);
        x = qMin(x, image.width() - 1);
        y = qMax(0, y);
        y = qMin(y, image.height() - 1);
        return image.pixel(x, y);
    }
    return QColor(Qt::black).rgb();
}

TetragonWindow::~TetragonWindow(){
    cleanup();
}

void TetragonWindow::resizeEvent(QResizeEvent * ){
    const int offsetWidth = this->width() - WIDTH_SETTINGS_PANEL - 1;
    puzzlePanel->setGeometry(QRect(QPoint(offsetWidth, 0), QPoint(offsetWidth + WIDTH_SETTINGS_PANEL, HEIGHT_SETTINGS_PANEL)));
    setTetragonArea();
    foreach(const Tetragon* tetragon, tetragons){
        drawTetragon(tetragonArea, tetragon);
    }
    foreach(const QPointF& boldVertex, userLastTetragon){
        drawBoldVertex(boldVertex);
    }

    drawOrigin();
}

void TetragonWindow::paintEvent(QPaintEvent *){
    QPainter painter(this);
    painter.drawImage(0, 0, tetragonArea);
    painter.drawImage(tetragonArea.width(), tetragonArea.height(), littleOriginImage);
}

void TetragonWindow::drawBoldVertex(QPointF vertex){
    int scaleVertexX = vertex.x() * tetragonArea.width();
    int scaleVertexY = vertex.y() * tetragonArea.height();
    for(int i = 0; i < PIXEL_BLOCK; ++i){
        for(int j = 0;j < PIXEL_BLOCK;++j){
            safeSetPixel(scaleVertexX, scaleVertexY, QColor(Qt::blue).rgb() );
            safeSetPixel(scaleVertexX + i, scaleVertexY + j, QColor(Qt::blue).rgb() );
            safeSetPixel(scaleVertexX + i, scaleVertexY - j, QColor(Qt::blue).rgb() );
            safeSetPixel(scaleVertexX - i, scaleVertexY + j,  QColor(Qt::blue).rgb() );
            safeSetPixel(scaleVertexX - i, scaleVertexY - j, QColor(Qt::blue).rgb() );
        }
    }
}

void TetragonWindow::mouseReleaseEvent(QMouseEvent * event){
    if(event->pos().x() >= tetragonArea.width() || event->pos().y() >= tetragonArea.height() ){
        return;
    }

    update();
    QPointF vertex = QPointF(
                static_cast<double>(event->pos().x()) / tetragonArea.width(),
                static_cast<double>(event->pos().y()) / tetragonArea.height());
    drawBoldVertex(vertex);
    userLastTetragon << vertex;
    counter++;
    if(counter == 4){
        counter = 0;
        bool convex = GeometricUtils::lineInterscets(userLastTetragon[0].x(), userLastTetragon[0].y(),
                                       userLastTetragon[1].x(), userLastTetragon[1].y(),
                                       userLastTetragon[2].x(), userLastTetragon[2].y(),
                                       userLastTetragon[3].x(), userLastTetragon[3].y());
        Tetragon*  tetragon  = 0;
        if(convex){
            tetragon = new ConvexTetragon(userLastTetragon[0], userLastTetragon[1],
                          userLastTetragon[2], userLastTetragon[3]);
        }
        else{
            tetragon = new NoConvexTetragon(userLastTetragon[0], userLastTetragon[1],
                          userLastTetragon[2], userLastTetragon[3]);
        }

        tetragons << tetragon;
        userLastTetragon.clear();
        drawTetragon(tetragonArea, tetragon);
    }
}


void TetragonWindow::calcMipLevels(){
    int levelLen = qMin(textureImage.width() , textureImage.height());
    int levelWidth = textureImage.width() ;
    int levelHeight = textureImage.height() ;
    QImage previousLevel = textureImage.copy();
    mipLevels.push_back(previousLevel);
    while(levelLen != 1){
        levelLen /= 2;
        levelWidth /= 2;
        levelHeight /= 2;
        QImage nextMipLevel = QImage(levelWidth, levelHeight, QImage::Format_RGB888);
        for(int i = 0; i < levelWidth; ++i){
            for(int j = 0; j < levelHeight; ++j){
                uint pix0 = previousLevel.pixel(i * 2, j * 2);
                uint pix1 = previousLevel.pixel(i * 2 + 1, j * 2);
                uint pix2 = previousLevel.pixel(i * 2, j * 2 + 1);
                uint pix3 = previousLevel.pixel(i * 2 + 1, j * 2 + 1);
                int red = (QColor(pix0).red() + QColor(pix1).red() + QColor(pix2).red() + QColor(pix3).red()) / 4;
                int green = (QColor(pix0).green() + QColor(pix1).green() + QColor(pix2).green() + QColor(pix3).green()) / 4;
                int blue = (QColor(pix0).blue() + QColor(pix1).blue() + QColor(pix2).blue() + QColor(pix3).blue()) / 4;
                nextMipLevel.setPixel(i, j, qRgb(red, green, blue));
            }
        }
        mipLevels.push_back(nextMipLevel);
        previousLevel = nextMipLevel.copy();
    }

}

void TetragonWindow::safeSetPixel(QImage& image, int x, int y, uint color){
    if(0 <= x && x < image.width() && 0 <= y && y < image.height()){
        image.setPixel(x, y, color);
    }
}

void TetragonWindow::safeSetPixel(int x, int y, uint color){
    if(0 <= x && x < tetragonArea.width() && 0 <= y && y < tetragonArea.height()){
        tetragonArea.setPixel(x, y, color);
    }
}

uint TetragonWindow::safeGetPixel(int x, int y){
    if(0 <= x && x <= textureImage.width() - 1 && 0 <= y && y <= textureImage.height() - 1 ){
        return textureImage.pixel(x, y);
    }
    return QColor(Qt::black).rgb();
}

void TetragonWindow::drawTetragonLine(QImage& image, int x1, int y1, int x2, int y2, QMap<int, QVector<QPair<int, int> > >& points, int id) {
    Qt::GlobalColor color = Qt::red;

    const int deltaX = abs(x2 - x1);
    const int deltaY = abs(y2 - y1);
    const int signX = x1 < x2 ? 1 : -1;
    const int signY = y1 < y2 ? 1 : -1;

    int error = deltaX - deltaY;
    safeSetPixel(image, x2, y2, QColor(color ).rgb());
    if(y1 < y2){
        points[y2].append(QPair<int, int>(x2, id));
    }
    else{
        points[y1].append(QPair<int, int>(x1, id));
    }
    bool registry = false;
    int i = 0;
    for(i = 0; (x1 != x2 || y1 != y2) && (i < DEADLINE) ; ++i) {
        safeSetPixel(image, x1, y1, QColor(color ).rgb());
        if(registry ){
            points[y1].append(QPair<int, int>(x1, id));
        }
        registry = true;
        const int error2 = error * 2;

        if(error2 > -deltaY) {
            error -= deltaY;
            x1 += signX;

        }
        if(error2 < deltaX) {
            error += deltaX;
            y1 += signY;

        }
    }
    assert(i < DEADLINE);
}

void TetragonWindow::sl_drawConvex(){
    genTetragon(ModeTetragon_Convex);
}

void TetragonWindow::sl_drawNoConvex(){
    genTetragon(ModeTetragon_NoConvex);
}

void TetragonWindow::sl_onClear(){
    tetragonArea.fill(QColor(Qt::gray).rgb());
    cleanup();
    foreach(const QPointF& boldVertex, userLastTetragon){
        drawBoldVertex(boldVertex);
    }
    update();
}

void TetragonWindow::cleanup(){
    foreach(Tetragon* t, tetragons){
        delete t;
    }
    tetragons.clear();
}

void TetragonWindow::sl_onFilterChanged(bool){
    if(simple->isChecked() ){
        filter = Filter_Simple;
    }
    else if(biLeneal->isChecked()){
        filter = Filter_Bileneal;
    }
    else{
        filter = Filter_Trilineal;
    }
    setTetragonArea();
    foreach(const Tetragon* tetragon, tetragons){
        drawTetragon(tetragonArea, tetragon);
    }
    drawOrigin();
}

struct TetragonLess{
    bool operator()(const QPair<int, int> a, const QPair<int, int> b){
        return a.first < b.first || (a.first == b.first && a.second < b.second);
    }
};

static double dist(double x1, double y1, double x2, double y2){
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

double TetragonWindow::findPixelLength(const Tetragon* tetragon, const QImage& image, int j, int i){
    bool ok = true;
    bool correct = true;
    // (find max pixel length in u,v. Considered on pixel with center of x,y and)
    // calc distance between that center and (x - 1, y) (x + 1, y) (x, y - 1), (x, y + 1)
    double u = tetragon->getU(static_cast<double>(j) / image.width(),
                          static_cast<double>(i)/ image.height(), &ok) * textureImage.width();
    correct = correct && ok;
    double v = tetragon->getV(static_cast<double>(j) / image.width(),
                             static_cast<double>(i)/ image.height(), &ok) * textureImage.height();
    correct = correct && ok;
    double u1 = tetragon->getU(static_cast<double>(j - 1) / image.width(),
                          static_cast<double>(i)/ image.height(), &ok) * textureImage.width();
    correct = correct && ok;
    double v1 = tetragon->getV(static_cast<double>(j - 1) / image.width(),
                             static_cast<double>(i )/ image.height(), &ok) * textureImage.height();
    correct = correct && ok;
    double u2 = tetragon->getU(static_cast<double>(j) / image.width(),
                          static_cast<double>(i - 1)/ image.height(), &ok) * textureImage.width();
    correct = correct && ok;
    double v2 = tetragon->getV(static_cast<double>(j) / image.width(),
                             static_cast<double>(i  - 1)/ image.height(), &ok) * textureImage.height();
    correct = correct && ok;

    double u3 = tetragon->getU(static_cast<double>(j + 1) / image.width(),
                          static_cast<double>(i)/ image.height(), &ok) * textureImage.width();
    correct = correct && ok;
    double v3 = tetragon->getV(static_cast<double>(j + 1) / image.width(),
                             static_cast<double>(i )/ image.height(), &ok) * textureImage.height();
    correct = correct && ok;
    double u4 = tetragon->getU(static_cast<double>(j ) / image.width(),
                          static_cast<double>(i + 1)/ image.height(), &ok) * textureImage.width();
    correct = correct && ok;
    double v4 = tetragon->getV(static_cast<double>(j) / image.width(),
                             static_cast<double>(i + 1)/ image.height(), &ok) * textureImage.height();
    if(!correct){
        return 1.;
    }
    double pixelLenght = 0.;
    pixelLenght = qMax(pixelLenght , dist(u, v, u1, v1));
    pixelLenght = qMax(pixelLenght , dist(u, v, u2, v2));
    pixelLenght = qMax(pixelLenght , dist(u, v, u3, v3));
    pixelLenght = qMax(pixelLenght , dist(u, v, u4, v4));
    return pixelLenght;
}

void TetragonWindow::genTetragon(ModeTetragon mode){
    Tetragon* tetragon = gen.getNext(mode);
    tetragons << tetragon;
    drawTetragon(tetragonArea, tetragon);
}

void TetragonWindow::sl_transformChanged(int state){
    if(Qt::Unchecked == state){
        gauss = true;

    }
    else{
        gauss = false;
    }
    setTetragonArea();
    foreach(const Tetragon* tetragon, tetragons){
        drawTetragon(tetragonArea, tetragon);
    }
    drawOrigin();
}

void TetragonWindow::setVertexNotFinded(const QImage& image, QMap<int, QVector<QPair<int, int> > >& points, const QVector<QPointF>& pointsTetragon){
    for(int i = 0; i < pointsTetragon .size() ; ++i){
        QPointF p = pointsTetragon .at(i) ;
        bool has = qFind(points[p.y() * image.height()].begin(),
                         points[p.y() * image.height()].end(),
              QPair<int, int> (p.x() * image.width(), i))
                != points[p.y() * image.height()].end();
        has = has || qFind(points[p.y() * image.height()].begin(),
                           points[p.y() * image.height()].end(),
                           QPair<int, int> (p.x() * image.width(),
                            (i + 1) % pointsTetragon.size()))
                             != points[p.y() * image.height()].end();
        has = has || qFind(points[p.y() * image.height()].begin(),
                           points[p.y() * image.height()].end(),
                           QPair<int, int> (p.x() * image.width(),
                            (i - 1) % pointsTetragon.size()))
                             != points[p.y() * image.height()].end();
        if(!has){
            points[p.y() * image.height()].append(QPair<int, int>(p.x() * image.width(), i));
            points[p.y() * image.height()].append(QPair<int, int>(p.x() * image.width(), (i + 1) % pointsTetragon.size()));
        }
    }
}

void TetragonWindow::drawTetragon(QImage& image, const Tetragon* tetragon){    
    // choose transform
    if(gauss){
        foreach(Tetragon* tetragon, tetragons){
            tetragon->setTransform();
        }
    }
    else{
        foreach(Tetragon* tetragon, tetragons){
            tetragon->setTransform2();
        }
    }

    QVector<QPointF> pointsTetragon;
    pointsTetragon << tetragon->getP0() << tetragon->getP1() <<
                                       tetragon->getP2() << tetragon->getP3();

    // draw vertex
    foreach(const QPointF& tetragonVertex, pointsTetragon){
        drawBoldVertex(tetragonVertex);
    }

    // draw edges and fill map with Y and mathced Vector<x, edge>
    QMap<int, QVector<QPair<int, int> > > points;
    for(int i = 0; i < pointsTetragon .size() ; ++i){
        QPointF from = pointsTetragon .at(i) ;
        QPointF to = pointsTetragon .at((i + 1) % pointsTetragon .size());
        drawTetragonLine(image, from.x() * image.width(),
                         from.y() * image.height(),
                         to.x() * image.width(),
                         to.y() * image.height(),
                         points, i);
    }

    setVertexNotFinded(image, points, pointsTetragon);

    int up =  tetragon->getUpMax() * image.height();
    int down = tetragon->getDownMin() * image.height();

    QVector< QPair<int, int> > drawedPoints;

    // filled interior of tetragon
    for(int i = up ; i >= down; --i){
        QVector<QPair<int, int> > pointsOnLine = points[i];
        qSort(pointsOnLine.begin(), pointsOnLine.end(), TetragonLess());
        if(pointsOnLine.size() == 0){
            continue;
        }
        int left = 0;
        int leftEnd = 0;
        int id = pointsOnLine.at(0).second;
        //find places to fill from left
        for(int k = 0; k < pointsOnLine.size() - 1; ++k){
            if(qAbs(pointsOnLine.at(k).first - pointsOnLine.at(k + 1).first) > 1){
                left = pointsOnLine.at(k).first + 1;
                leftEnd = pointsOnLine.at(k + 1).first;
                break;
            }
            int newId = pointsOnLine.at(k + 1).second;
            if(newId != id){
                break;
            }
        }
        // filled from left
        for(int j = left; j < leftEnd; ++j){
            drawedPoints.push_back(QPair<int, int>(j, i));
        }

        int right = 0;
        int rightEnd = 0;
        id = pointsOnLine.at(pointsOnLine.size() - 1).second;
        // find places to fill from right
        for(int k = pointsOnLine.size() - 1; k > 0; --k){
            if(qAbs(pointsOnLine.at(k).first - pointsOnLine.at(k - 1).first) > 1){
                right = pointsOnLine.at(k).first - 1;
                rightEnd = pointsOnLine.at(k - 1).first;
                break;
            }
            int newId = pointsOnLine.at(k - 1).second;
            if(newId != id){
                break;
            }
        }
        // filled from right
        for(int j = right; j > rightEnd; --j){
            drawedPoints.push_back(QPair<int, int>(j, i));
        }
    }

    // deffered drawing
    for(int i = 0; i <  drawedPoints.size(); ++i){
        QPair<int, int> pair = drawedPoints[i];
        int j = pair.first;
        int k = pair.second;

        double maxPixelLength = findPixelLength(tetragon, image, j, k);
        double u = tetragon->getU(static_cast<double>(j) / image.width(),
                              static_cast<double>(k)/ image.height());
        double v = tetragon->getV(static_cast<double>(j) / image.width(),
                                 static_cast<double>(k)/ image.height());

        double num_mip_level = log2( maxPixelLength);
        int num_lower_mip_level = 0;
        int num_upper_mip_level = 0;

        if(maxPixelLength < 1){
            num_lower_mip_level = 0;
            num_upper_mip_level = num_lower_mip_level;
        }
        else if(num_mip_level >  mipLevels.size() - 1){
            num_lower_mip_level = mipLevels.size() - 1;
            num_upper_mip_level = num_lower_mip_level;
        }
        else{
            num_lower_mip_level = num_mip_level;
            num_upper_mip_level = num_lower_mip_level + 1;
        }

        if(filter == Filter_Simple){
            safeSetPixel(image,  j, k,
            safeGetPixel(textureImage, static_cast<double>(u * textureImage.width()) + 0.5,
                         static_cast<double> (v * textureImage.height())+ 0.5));
        }
        else if(filter == Filter_Bileneal){
            /*QImage imageTest = QImage(2, 2, QImage::Format_RGB888);
            imageTest.setPixel(0, 0, qRgb(255, 0, 0));
            imageTest.setPixel(0, 1, qRgb(0, 255, 0));
            imageTest.setPixel(1, 0, qRgb(0, 0, 255));
            imageTest.setPixel(1, 1, qRgb(127, 127, 127));

            const int size = 8;
            for(int i = 0; i < size ; ++i){
                for(int j = 0; j < size ; ++j){
                    double u1 = ((double)i ) / size ;
                    double v1 = (double(j )) / size ;
                    uint biLeneal = makeFilter(u1, v1, imageTest);
                    safeSetPixel(image, i, j, biLeneal);
                }
            }*/

            uint biLeneal = makeFilter(u, v, textureImage);
            safeSetPixel(image, j, k, biLeneal);
        }
        else{
            uint trilinearColor = trilinearFilter(num_lower_mip_level ,
                                                  num_upper_mip_level,
                                                  u, v,
                                                  num_mip_level - static_cast<int>(num_mip_level ));
            safeSetPixel(image, j, k, trilinearColor);
        }
    }

    update();
}


QRgb TetragonWindow::trilinearFilter(int levelLow, int levelUpper, double u, double v,  double betweenLevels){
    QImage lowMip = mipLevels.at(levelLow);
    QRgb lower = makeFilter(u, v, lowMip);
    QImage upperMip = mipLevels.at(levelUpper);
    QRgb upper = makeFilter(u, v, upperMip);

    int red = QColor(lower).red() * (1 - betweenLevels) + QColor(upper).red() * betweenLevels;
    int green = QColor(lower).green() * (1 - betweenLevels) + QColor(upper).green() * betweenLevels;
    int blue = QColor(lower).blue() * (1 - betweenLevels) + QColor(upper).blue() * betweenLevels;
    return qRgb(red, green, blue);
}

QRgb TetragonWindow::makeFilter(double u, double v, const QImage& filterImage){
    float newCoordX = u * (filterImage.width() - 1);
    int roundedNewCoordX = static_cast<int>(newCoordX);
    if(roundedNewCoordX == (filterImage.width() - 1)) {roundedNewCoordX --;}
    float shiftX = newCoordX - roundedNewCoordX;

    float newCoordY = v * (filterImage.height() - 1);
    int roundedNewCoordY = static_cast<int>(newCoordY);
    if(roundedNewCoordY == (filterImage.height() - 1)){roundedNewCoordY--;}
    float shiftY = newCoordY - roundedNewCoordY;

    int red = QColor(safeGetPixel(filterImage, roundedNewCoordX , roundedNewCoordY)).red() * (1 - shiftX) * (1 - shiftY) +
              QColor(safeGetPixel(filterImage, roundedNewCoordX + 1, roundedNewCoordY)).red() *  shiftX * (1 - shiftY) +
              QColor(safeGetPixel(filterImage, roundedNewCoordX + 1 , roundedNewCoordY + 1)).red() * shiftX * shiftY +
              QColor(safeGetPixel(filterImage, roundedNewCoordX  , roundedNewCoordY + 1)).red() * (1 - shiftX) * shiftY ;
    int green = QColor(safeGetPixel(filterImage, roundedNewCoordX , roundedNewCoordY)).green() * (1 - shiftX) * (1 - shiftY) +
              QColor(safeGetPixel(filterImage, roundedNewCoordX +1 , roundedNewCoordY )).green() *  shiftX * (1 - shiftY) +
              QColor(safeGetPixel(filterImage, roundedNewCoordX + 1 , roundedNewCoordY + 1)).green() * shiftX * shiftY +
              QColor(safeGetPixel(filterImage, roundedNewCoordX , roundedNewCoordY +1 )).green() * (1 - shiftX) * shiftY ;
    int blue = QColor(safeGetPixel(filterImage, roundedNewCoordX , roundedNewCoordY)).blue() * (1 - shiftX) * (1 - shiftY) +
              QColor(safeGetPixel(filterImage, roundedNewCoordX +1, roundedNewCoordY )).blue() *  shiftX * (1 - shiftY) +
              QColor(safeGetPixel(filterImage, roundedNewCoordX + 1 , roundedNewCoordY + 1)).blue() * shiftX * shiftY +
              QColor(safeGetPixel(filterImage, roundedNewCoordX  , roundedNewCoordY + 1 )).blue() * (1 - shiftX) * shiftY ;
    return qRgb(red, green, blue);
}


void TetragonWindow::drawOrigin(){
    assert(littleOriginImage.width() == littleOriginImage.height());

    double numTexsels = static_cast<double>(textureImage.width()) / littleOriginImage.width();
    double num_mip_level = log2(numTexsels);

    int num_lower_mip_level = 0;
    int num_upper_mip_level = 0;
    if(numTexsels < 1){
        num_lower_mip_level = 0;
        num_upper_mip_level = num_lower_mip_level;
    }
    else if(num_mip_level >  mipLevels.size() - 1){
        num_lower_mip_level = mipLevels.size() - 1;
        num_upper_mip_level = num_lower_mip_level;
    }
    else{
        num_lower_mip_level = num_mip_level;
        num_upper_mip_level = num_lower_mip_level + 1;
    }

    for(int i = 0; i < littleOriginImage.width(); ++i){
        for(int j = 0; j < littleOriginImage.height(); ++j){
            double u = static_cast<double>(i ) / littleOriginImage.width();
            double v =static_cast<double> (j ) / littleOriginImage.height() ;
            uint trilinearColor = trilinearFilter(num_lower_mip_level ,
                                              num_upper_mip_level,
                                              u, v,
                                              num_mip_level - static_cast<int>(num_mip_level ));
            if(filter == Filter_Simple){
                safeSetPixel(littleOriginImage, i, j,
                safeGetPixel(textureImage, static_cast<double>(i * textureImage.width()) / littleOriginImage.width() + 0.5,
                             static_cast<double> (j * textureImage.height()) / littleOriginImage.height()));
            }
            else if(filter == Filter_Bileneal){
                uint biLeneal = makeFilter(u, v, textureImage);
                safeSetPixel(littleOriginImage, i, j, biLeneal);
            }
            else{
                safeSetPixel(littleOriginImage, i, j, trilinearColor);
            }
        }
    }

    update();
}

