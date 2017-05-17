#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Methode 1.
    // --------------------------------------- //
    // w.show();
    // w.move((QApplication::desktop()->width() - w.width())/2,(QApplication::desktop()->height() - w.height())/2);

    // Methode 2.
    // --------------------------------------- //
    w.showMaximized();

    return a.exec();
}
