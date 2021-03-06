﻿#include "bodhisession.h"
#include "bodhiapp.h"
#include "config.h"
#include "bodhicore.h"
#include "bodhiplayer.h"
#include "srtview.h"
#include "srtarchive.h"
#include "parser.h"
#include <QFileInfo>
#include <QDebug>
#include <functional>
#include <QTextCodec>
#include "commands.h"
#include <algorithm>
#include <memory>
#pragma execution_character_set("utf-8")

BodhiSession::BodhiSession(const Work &work, QObject *parent)
    : QObject(parent)
    , m_work(work)
    , m_player(NULL)
    , m_view(NULL)
    , m_data(NULL)
    , m_active(true)
    , m_lastError(BS_OK)
{
}

BodhiSession::~BodhiSession()
{
    if (m_data && m_data->changed())
        saveSrtFile(*m_data, m_work.subtitleFile);
    close();
}

bool BodhiSession::open(){
    if (!doOpen()){
        clean();
        return false;
    }
    qInfo() << "session open";
    QFileInfo fi(m_work.subtitleFile);
    m_label = fi.fileName();
    m_label = m_label.mid(0, m_label.lastIndexOf('.'));
    return true;
}

bool BodhiSession::doOpen()
{
    WorkType type = m_work.type;
    if (type == WT_DICATION){
        if (!createPlayer(m_work.soundFile))
            return false;

        if (m_work.subtitleFile.isEmpty()){
            m_work.subtitleFile = getMatchedPath(m_work.soundFile);
            m_data = new BodhiSubtitle();
            connect(m_player->getQtPlayer(), SIGNAL(durationChanged(qint64)), this, SLOT(onDurationGot(qint64)));
        } else {
            if (!loadSrtFile(m_work.subtitleFile))
                return false;
        }
        onDurationGot(m_player->duration());
        return true;
    } else if (type == WT_COLLATION) {
        if (!createPlayer(m_work.soundFile))
            return false;
    }
    return false;
}

void BodhiSession::onDurationGot(qint64 duration)
{
    qInfo() << "session got duration: " << duration;

//    if (m_data->recordCount() == 0){
//        SrtRecord record = SrtRecord(0, 0, duration);
//        m_data->append(record);

//        if (m_view){
//            qInfo() << "notify udpate!";
//            m_view->update();
//        }
//    }
}

void BodhiSession::onCommand(Command *command)
{
    if(!command)
        return;

    if(command->executeFlag() == Command::E_NewTask){
        m_undoList.push_back(command);
        clearCommandHistory(E_RedoList);
    }

    m_view->onCommand(command);
}

void BodhiSession::clearCommandHistory(CommandListType type)
{
    auto fn = [](Command *cmd) {delete cmd;};
    if (type & E_UndoList){
        std::for_each(m_undoList.begin(), m_undoList.end(), fn);
    }
    if (type & E_RedoList){
        std::for_each(m_redoList.begin(), m_redoList.end(), fn);
    }
}

void BodhiSession::clean()
{
    if (m_data){
        delete m_data;
        m_data = NULL;
    }

    if (m_player){
        m_player->close();
        delete m_player;
        m_player = NULL;
    }

    m_label = "";
}

void BodhiSession::close()
{
    //TODO:
    if (!m_data)
        return;

    if (m_data->changed()){
        save();
    }

    if (m_player){
        m_player->close();
    }
    emit sessionEnd(this);
    clean();
}

void BodhiSession::save()
{
    saveSrtFile(*m_data, m_work.subtitleFile);
    Config config = BodhiApp::getInstance().getConfig();
    config.addHistory(m_work);
    config.saveHistory();
}

void BodhiSession::setView(SrtView *view)
{
    m_view = view;
    if (view){
        connect(this, SIGNAL(sessionStart(BodhiSession*)), view, SLOT(onSessionStart(BodhiSession*)));
        connect(this, SIGNAL(sessionEnd(BodhiSession*)), view, SLOT(onSessionEnd(BodhiSession*)));
        emit sessionStart(this);
        view->update();
    }
}

BodhiSubtitle* BodhiSession::loadSrtFile(const QString &path)
{
    QFileInfo srtFile(path);
    SrtArchive archive(path);
    if (!archive.load()){
        qDebug() << "open srt file: " << path << " failed.";
        m_lastError = ERR_LOAD_SRT_FILE;
        return NULL;
    }
    SrtParser parser(archive);
    BodhiSubtitle *subtitle = new BodhiSubtitle();
    while (true){
        std::auto_ptr<SrtRecord> record(new SrtRecord(0));
        if (!parser.parseOne(record.get()))
            break;

        SrtRecordPtr r(std::move(record));
        if (!subtitle->append(r)){
            qDebug() << "append record failed!";
            delete subtitle;
            m_lastError = ERR_LOAD_SRT_FILE;
            return NULL;
        }
    }
    if (parser.getErrorString() != ""){
//        qDebug() << tr("show chinese tr: 中文");
//        QString tmp(tr("中文1"));
//        QString tmp2(("中文2"));
//        QString tmp3 = QString("第%1行：未读到字幕文本。").arg(22);
//        qDebug() << tmp.toUtf8().constData();
//        qDebug() << tmp2.toUtf8().constData();
//        qWarning() << tmp3.toUtf8().constData();
        qWarning() << parser.getErrorString().toUtf8().constData();
        m_lastError = ERR_LOAD_SRT_FILE;
        return NULL;
    }

    m_label = srtFile.fileName();
    m_data = subtitle;
    return m_data;
}

bool BodhiSession::saveSrtFile(BodhiSubtitle &data, const QString &path)
{
    SrtArchive archive(path);
    if (!archive.write(data, path)){
        qDebug() << "open srt file: " << path << " failed.";
        return false;
    }
    data.resetChangeFlag();
    return true;
}

BodhiPlayer* BodhiSession::createPlayer(const QString &path)
{
    if (m_player){
        qWarning() << "session: player already exist!";
        return m_player;
    }
    BodhiPlayer *player = new BodhiPlayer();
    auto fn = std::bind(&BodhiSession::onDurationGot, this, std::placeholders::_1);
    if (!player->open(path, fn)){
        delete player;
        m_lastError = ERR_OPEN_AUDIO_FILE;
        return NULL;
    }
    m_player = player;
    return m_player;
}

void BodhiSession::active()
{
    m_active = true;
    qDebug() << "session actived: " << m_view->tabIndex();
}

void BodhiSession::deactive()
{
    qDebug() << "session deactived: " << m_view->tabIndex();
    m_active = false;
    if (m_player && m_player->isPlaying())
        m_player->pause();
}

void BodhiSession::on_btnUndo_clicked()
{

}

void BodhiSession::on_btnRedo_clicked()
{

}

void BodhiSession::on_btnAdd_clicked()
{
    qDebug() << "add record button clieck.";
    if (!m_player->isOpen())
        return;

    qint64 pos, len;
    pos = m_player->position();
    len = m_player->duration();

    AddRecordCommand *command = new AddRecordCommand(*this, pos, len);
    command->execute();
    onCommand(command);
}

void BodhiSession::on_btnRemove_clicked()
{

}

void BodhiSession::on_btnSplit_clicked()
{

}

void BodhiSession::on_btnMerge_clicked()
{

}

void BodhiSession::on_btnTimeDec_clicked()
{

}

void BodhiSession::on_btnTimeInc_clicked()
{

}
