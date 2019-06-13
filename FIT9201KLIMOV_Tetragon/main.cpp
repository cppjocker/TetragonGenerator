#include <QtGui/QApplication>
#include "tetragonwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TetragonWindow w;
    w.show();
    return a.exec();
}
