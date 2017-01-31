#ifndef BODHIPLAYER_H
#define BODHIPLAYER_H

#include <QObject>
#include <QWidget>
#include <QMediaPlayer>
#include <functional>

class BodhiPlayer : public QObject
{
    Q_OBJECT

private:
    QMediaPlayer *m_player;
    qint64 m_start;
    qint64 m_end;
    qint64 m_duration;
    int m_volume;
    int m_notifyInterval;
    std::function<void(qint64)> m_cbDurationChanged;
public:
    BodhiPlayer(QObject *parent = 0);
    ~BodhiPlayer();

    bool open(const QString &path, std::function<void(qint64)> cb);

    bool play();
    bool play(qint64 start, qint64 end);
    bool isPlaying() const;
    bool isOpen() const;
    void pause();
    void playPause();

    void close();

    void setVolume(int vol);
    qint64 duration() const;
    qint64 position() const;
    QMediaPlayer* getQtPlayer() const { return m_player; }

public slots:
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
signals:

};

#endif // BODHIPLAYER_H
