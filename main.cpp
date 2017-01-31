#include "mainwindow.h"
#include <QApplication>
#include "qfiledialog.h"
#include "qdir.h"
#include "qdebug.h"
#include "config.h"
#include <QTextCodec>
#pragma execution_character_set("utf-8")
#pragma comment(lib, "User32.lib")

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.onStart(a);

    return a.exec();
}
