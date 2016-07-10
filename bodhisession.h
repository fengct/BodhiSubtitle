#ifndef BODHISESSION_H
#define BODHISESSION_H

#include <QObject>
#include "config.h"

class BodhiPlayer;
class SrtView;
class BodhiSubtitle;

class BodhiSession : public QObject
{
    Q_OBJECT

public:
    explicit BodhiSession(const Work &work, QObject *parent = 0);
    ~BodhiSession();

    bool open();
    void close();
    void save();

    BodhiPlayer* player() const { return m_player; }
    SrtView* view() const { return m_view; }
    BodhiSubtitle* subtitle() const { return m_data; }
    QString label() const { return m_label; }

    void setView(SrtView *view);
    Work getWorkInfo() const { return m_work; }

    void active();
    void deactive();
    bool isActive() const { return m_active; }
signals:
    void sessionStart(BodhiSession *session);
    void sessionEnd(BodhiSession *session);

public slots:
    void onDurationGot(qint64 duration);
    void on_btnUndo_cliccked();
    void on_btnRedo_clicked();
    void on_btnAdd_cliecked();
    void on_btnRemove_clicked();
    void on_btnSplit_clicked();
    void on_btnMerge_clicked();
    void on_btnTimeDec_clicked();
    void on_btnTimeInc_clicked();
protected:
    BodhiSubtitle* loadSrtFile(const QString &path);
    bool saveSrtFile(BodhiSubtitle &data, const QString &path);
    BodhiPlayer* createPlayer(const QString &path);
    bool doOpen();
    void clean();
private:
    Work m_work;
    BodhiPlayer *m_player;
    SrtView *m_view;
    BodhiSubtitle *m_data;
    QString m_label;
    bool m_active;
};

#endif // BODHISESSION_H
