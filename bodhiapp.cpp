#include "bodhiapp.h"
#include "qdir.h"
#include "qfiledialog.h"
//#include "const.h"
#include "mainwindow.h"
#include "bodhisession.h"
#include <algorithm>
#pragma execution_character_set("utf-8")


BodhiApp* BodhiApp::m_inst = NULL;

BodhiApp& BodhiApp::getInstance()
{
    Q_ASSERT(m_inst != NULL);
    return *m_inst;
}

BodhiApp::BodhiApp(MainWindow &mainWnd, QApplication &app)
    : m_mainWnd(mainWnd)
    , m_app(app)
    , m_activeSession(NULL)
{
    Q_ASSERT(!m_inst);
    m_inst = this;
}

void BodhiApp::onAppStart()
{
    if(m_config.isFirstTimeRun()){
        qDebug() << "Determined first time run this app!";

        QString path = m_config.getCfgDir();
        QDir dir(path);
        if (!dir.mkdir(path)){
            qWarning() << "Create dir <" << path << "> failed.";
            return;
        }

        QFileDialog dlg;
        //QDir dir = dlg.getExistingDirectory();
        QDir wdir = QFileDialog::getExistingDirectory(&m_mainWnd, "Choose workspace");
        //qDebug("---start");
        qDebug() << "Set work dir: " << wdir.absolutePath();
        m_config.setWorkDir(wdir.absolutePath());
        m_config.saveHistory();
    } else {
        m_config.readHistory();
    }
}

void BodhiApp::onAppStop()
{

}

bool BodhiApp::fileIsOpen(const QString &path)
{
    SessionList::iterator it = m_sessions.begin();
    for(; it != m_sessions.end(); it++){
        BodhiSession *session = *it;
        Work w = session->getWorkInfo();
        if (w.subtitleFile == path)
            return true;
    }
    return false;
}

BodhiSession* BodhiApp::createSession(Work &work)
{
    BodhiSession *session = new BodhiSession(work, &m_mainWnd);
    if (!session->open()){
        delete session;
        return NULL;
    }
    if (m_activeSession){
        m_activeSession->deactive();
    }
    m_activeSession = session;
    m_sessions.push_back(session);
    return session;
}

void BodhiApp::closeSession(BodhiSession* session)
{
    if (!session){
        qWarning() << "closesession, null ptr.";
        return;
    }

    session->close();
    SessionList::iterator it = std::find(m_sessions.begin(), m_sessions.end(), session);
    m_sessions.erase(it);
    if (session == m_activeSession){
        if (m_sessions.empty())
            m_activeSession = NULL;
        else
            m_activeSession = *m_sessions.rbegin();
    }
}

void BodhiApp::closeCurrentSession()
{
    closeSession(m_activeSession);
}

BodhiSession* BodhiApp::activeSession() const
{
    return m_activeSession;
}

BodhiSession* BodhiApp::setActiveSession(BodhiSession *session)
{
    BodhiSession *old = m_activeSession;
    if (old)
        old->deactive();
    if (session)
        session->active();
    m_activeSession = session;
    return old;
}
