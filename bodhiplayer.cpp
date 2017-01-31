#include "bodhiplayer.h"
#include <QUrl>
#pragma execution_character_set("utf-8")

BodhiPlayer::BodhiPlayer(QObject *parent)
    : QObject(parent)
    , m_player(NULL)
    , m_start(0)
    , m_end(0)
    , m_duration(-1)
    , m_volume(50)
    , m_notifyInterval(50)
{

}

BodhiPlayer::~BodhiPlayer()
{
    if (m_player){
        delete m_player;
        m_player = NULL;
    }
}

bool BodhiPlayer::open(const QString &path, std::function<void(qint64)> cb)
{
    if(m_player){
        m_player->pause();
        return true;
    }

    qInfo() << "open media file: " << path;
    m_player = new QMediaPlayer();
    m_cbDurationChanged = cb;
    connect(m_player, SIGNAL(positionChanged(qint64)), this, SLOT(onPositionChanged(qint64)));
    connect(m_player, SIGNAL(durationChanged(qint64)), this, SLOT(onDurationChanged(qint64)));
    m_player->setMedia(QUrl::fromLocalFile(path));
    m_player->setVolume(m_volume);
    m_start = 0;
    m_player->setNotifyInterval(20);
    m_player->pause();
    return true;
}

bool BodhiPlayer::play()
{
    if(m_player == NULL){
        qCritical() << "play: player not open!";
        return false;
    }
    qInfo() << "play.";
    m_player->setNotifyInterval(m_notifyInterval);
    m_player->play();
    return true;
}

bool BodhiPlayer::play(qint64 start, qint64 end)
{
    if(m_player == NULL){
        qCritical() << "play[start, end]: player not open!";
        return false;
    }

    qDebug() << "play [" << start << "] --- [" << end << "]\n";
    m_start = start;
    m_end = end;
    m_player->setPosition(start);
    m_player->setNotifyInterval(m_notifyInterval);
    m_player->play();
    return true;
}

bool BodhiPlayer::isPlaying() const
{
    if(m_player == NULL)
        return false;
    return m_player->state() == QMediaPlayer::PlayingState;
}

bool BodhiPlayer::isOpen() const
{
    if(m_player == NULL)
        return false;
    return m_player->state() != QMediaPlayer::StoppedState;
}

void BodhiPlayer::pause()
{
    if(m_player == NULL){
        qCritical() << "pause: player not open!";
        return;
    }
    qInfo() << "pause.";
    m_player->pause();
}

void BodhiPlayer::playPause()
{
    if(m_player == NULL){
        qCritical() << "playPause: player not open!";
        return;
    }

    qInfo() << "playPause.";
    if (m_player->state() == QMediaPlayer::PlayingState)
        m_player->pause();
    else if(m_player->state() == QMediaPlayer::PausedState)
        m_player->play();
}

void BodhiPlayer::close()
{
    if(m_player == NULL){
        qWarning() << "close: player not open!";
        return ;
    }
    m_player->stop();
    delete m_player;
    m_player = NULL;
}

qint64 BodhiPlayer::duration() const
{
    if(m_player == NULL){
        qWarning() << "duration: player not open!";
        return 0;
    }
    //return m_player->duration();
    qDebug() << "player duration=" << m_duration;
    return m_duration;
}

qint64 BodhiPlayer::position() const
{
    if(m_player == NULL){
        qWarning() << "postion: player not open!";
        return 0;
    }
    return m_player->position();
}

void BodhiPlayer::onPositionChanged(qint64 position)
{
    if (false)
        qDebug() << "pos: " << position;

    if (!m_end)
        m_end = m_player->duration();

    if (position+10 >= m_end){
        m_player->setPosition(m_start);
    }
}

void BodhiPlayer::onDurationChanged(qint64 duration)
{
    qInfo() << "duration changed, new value=" << duration << ", old value=" << m_duration;
    m_duration = duration;
    if (m_cbDurationChanged)
        m_cbDurationChanged(duration);
}
