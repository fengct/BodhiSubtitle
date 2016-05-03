#include "bodhiapp.h"
#include "qdir.h"
#include "qfiledialog.h"

BodhiApp* BodhiApp::m_inst = NULL;

BodhiApp::BodhiApp(MainWindow &mainWnd, QApplication &app)
    : m_mainWnd(mainWnd)
    , m_app(app)
{
    Q_ASSERT(!m_inst);
    m_inst = this;
}

void BodhiApp::onAppStart()
{
    if(m_config.isFirstTimeRun()){
        QFileDialog dlg;
        QDir dir = dlg.getExistingDirectory();
        //QDir dir = dlg.getSaveFileName();
        //qDebug("---start");
        qDebug() << dir.absolutePath();
        m_config.setWorkDir(dir.absolutePath());
        m_config.saveHistory();
    }
}

void BodhiApp::onAppStop()
{

}
