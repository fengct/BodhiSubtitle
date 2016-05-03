#ifndef BODHISESSION_H
#define BODHISESSION_H

#include <QObject>
#include "config.h"

class BodhiSession : public QObject
{
    Q_OBJECT

    Work m_workData;
public:
    explicit BodhiSession(QObject *parent = 0);
    bool start(WorkType type, Work *workData = NULL);
signals:

public slots:
};

#endif // BODHISESSION_H
