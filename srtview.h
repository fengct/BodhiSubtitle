#ifndef SRTVIEW_H
#define SRTVIEW_H

#include <QWidget>
#include <QScrollBar>
#include <QMediaPlayer>
#include "mainwindow.h"

class BodhiSubtitle;
class BodhiSession;
class Command;

class MyScrollBar : public QScrollBar
{
    Q_OBJECT
public slots:
    void onWheelEvent(QWheelEvent *event);
};

class SrtView : public QWidget
{
    Q_OBJECT

    Ui::MainWindow *m_ui;
    BodhiSubtitle *m_subtitle;
    BodhiSession *m_session;
    QPoint m_marginLT;
    QPoint m_marginRB;
    QScrollBar *m_scrollBar;
    int m_LH;
    int m_PH;
    int m_tsPos;
    int m_tePos;
    int m_contentPos;
    int m_tabIndex;
    int m_playingIndex;
    int m_selectStart;
    int m_selectCount;
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    //void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
    //void mouseMoveEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent * event) Q_DECL_OVERRIDE;
    //void keyReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
public:
    explicit SrtView(QWidget *parent, Ui::MainWindow *ui, BodhiSubtitle *subtitle, BodhiSession *session);
    void setTabIndex(int idx);
    int tabIndex() const { return m_tabIndex; }
    BodhiSession* session() const { return m_session; }
    void onCommand(Command *command);
    int singleSelectItem() const;
signals:
    void wheelEventFired(QWheelEvent *event);
public slots:
    void onSessionStart(BodhiSession *session);
    void onSessionEnd(BodhiSession *session);
    void onTabActived();
    void onScrolled(int pos);
    void onMediaPositionChanged(qint64 pos);
    void onMediaStateChanged(QMediaPlayer::State state); 
private:
    int pickRecord(int x, int y);
    int shiftSelectItem(int offset);
    void scroll(int offset);
    void scrollTo(int pos);
    int indexToPos(int index);
    bool inVisibleArea(int sequence);
    void updatePlayButtonText();
    void updateProgressBar(bool setRange);
};

#endif // SRTVIEW_H
