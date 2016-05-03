#include "mainwindow.h"
#include <QApplication>
#include "qfiledialog.h"
#include "qdir.h"
#include "qdebug.h"
#include "config.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.onStart(a);

    return a.exec();
}
