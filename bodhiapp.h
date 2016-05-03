#ifndef BODHIAPP_H
#define BODHIAPP_H

#include "config.h"

class MainWindow;
class QApplication;

class BodhiApp
{
    MainWindow &m_mainWnd;
    QApplication &m_app;
    Config m_config;
    static BodhiApp *m_inst;
public:
    BodhiApp(MainWindow &mainWnd, QApplication &app);

    static BodhiApp& getInstance();

    Config& getConfig() { return m_config; }

    void onAppStart();
    void onAppStop();
};

#endif // BODHIAPP_H
