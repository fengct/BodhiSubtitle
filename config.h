#ifndef CONFIG_H
#define CONFIG_H

#include <list>
//#include <QtGlobal>
#include "qdebug.h"
#include "qstring.h"
#include "qstringlist.h"
#include "qdatetime.h"
//#include "qglobal.h"
//#include "qglobalstatic.h"
#include "qjsondocument.h"

enum WorkType
{
    WT_UNKOWN,
    WT_DICATION,
    WT_COLLATION
};

struct Work{
    QString soundFile;
    QString subtitleFile;
    WorkType type;
    QDateTime firstTime;
    QDateTime lastTime;
    QTime usedTime;
    int lastIndex;

public:
    Work();
};
typedef std::list<Work> WorkList;
QJsonObject* Work2Json(const Work &w, QJsonObject *json);
bool WorkFromJson(QJsonObject &json, Work *w);

class Config
{
    WorkList m_history;
    QString m_workDir;
public:
    bool readHistory();
    bool saveHistory();
    const WorkList& getHistory() const { return m_history; }
//    const QStringList getHistoryFiles();
    void addHistory(Work &w);
    bool isFirstTimeRun();
    void setWorkDir(const QString &dir) { m_workDir = dir; }
    QString getWorkDir() const { return m_workDir; }
    QString getCfgDir();
protected:
};

QString getMatchedPath(const QString &path);

#endif // CONFIG_H

