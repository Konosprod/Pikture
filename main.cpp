#include <QApplication>
#include "mainwindow.h"
#include <QtGui>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    mainWindow w;
    w.show();
    
    return a.exec();
}
