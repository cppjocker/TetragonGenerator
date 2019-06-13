#ifndef PUZZLEWINDOW_H
#define PUZZLEWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QTimer>
#include <QTime>
#include <QMap>
#include <QPair>

#include "ui_mainwindow.h"
#include "tetragongenerator.h"



enum Filter{
    Filter_Simple,
    Filter_Bileneal,
    Filter_Trilineal
};

class TetragonWindow : public QMainWindow, public  Ui_TetragonWindow
{
    Q_OBJECT
public:
    explicit TetragonWindow(QWidget *parent = 0);
    ~TetragonWindow();
protected:
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent *);
    void mouseReleaseEvent(QMouseEvent *);
private slots:
    void sl_drawConvex();
    void sl_drawNoConvex();
    void sl_onClear();
    void sl_onFilterChanged(bool);
    void sl_transformChanged(int);
private:
    // draw bold point around place user mouse clicked
    void drawBoldVertex(QPointF vertex);    
    void setTetragonArea();
    // draw line between tetragon vertex and fiil map with Y coordinates with matched vector of X coordinates of specified edge
    void drawTetragonLine(QImage& image, int, int, int, int, QMap<int, QVector<QPair<int, int> > > &pointsEdge, int);
    // if vertex was not marked -> we marked it two times
    void setVertexNotFinded(const QImage& image, QMap<int, QVector<QPair<int, int> > >& points, const QVector<QPointF>&);
    // find max length of pixel in u,v coordinates
    double findPixelLength(const Tetragon*, const QImage&, int, int);
    void safeSetPixel(int x, int y, uint color);
    void safeSetPixel(QImage&, int x, int y, uint color);
    uint safeGetPixel(int x, int y);
    uint safeGetPixel(const QImage& image, int x, int y);
    // generate random tetragon of required mode
    void genTetragon(ModeTetragon mode);
    void drawTetragon(QImage& image, const Tetragon*);
    void calcMipLevels();
    // draw picture in left lower corner
    void drawOrigin();
    void cleanup();
    // bilineal
    QRgb makeFilter(double u, double v, const QImage& filterImage);
    // trilineal
    QRgb trilinearFilter(int levelLow, int levelUpper, double u, double v, double betweenLevels);
private:
    TetragonGenerator gen;
    QImage textureImage;
    QImage tetragonArea;
    QImage littleOriginImage;
    QVector<QPointF> userLastTetragon;
    int counter;
    QVector<QImage> mipLevels;
    Filter filter;
    QVector<Tetragon*> tetragons;
    bool gauss;
};

#endif // PUZZLEWINDOW_H
