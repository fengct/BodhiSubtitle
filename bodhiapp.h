#ifndef BODHIAPP_H
#define BODHIAPP_H

#include <list>
#include "config.h"

class MainWindow;
class QApplication;
class BodhiSession;

class BodhiApp
{
    MainWindow &m_mainWnd;
    QApplication &m_app;
    Config m_config;
    typedef std::list<BodhiSession*> SessionList;
    SessionList m_sessions;
    BodhiSession *m_activeSession;
    static BodhiApp *m_inst;
public:
    BodhiApp(MainWindow &mainWnd, QApplication &app);

    static BodhiApp& getInstance();

    Config& getConfig() { return m_config; }

    void onAppStart();
    void onAppStop();

    bool fileIsOpen(const QString &path);

    BodhiSession* createSession(Work &work);
    void closeSession(BodhiSession* session);
    void closeCurrentSession();
    BodhiSession* activeSession() const;
    BodhiSession* setActiveSession(BodhiSession *session);
};

#endif // BODHIAPP_H
