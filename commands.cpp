#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "commands.h"
#include "bodhisession.h"
#include "srtview.h"

Command::Command(BodhiSession &session)
    : m_session(session)
    , m_execFlag(E_NewTask)
    , m_activeLine(-1)
{
    m_activeLine = session.view()->singleSelectItem();
}


AddRecordCommand::AddRecordCommand(BodhiSession &session, qint64 pos, qint64 length)
    : Command(session)
    , m_pos(pos)
    , m_length(length)
{
}

void AddRecordCommand::execute()
{
    qDebug() << "add record command executing...";
    BodhiSubtitle *subtitle = m_session.subtitle();
    Q_ASSERT(subtitle);

    int cnt = subtitle->recordCount();
    SrtRecordPtr rec(new SrtRecord(cnt));
    if (cnt > 0){
        SrtRecordPtr last = subtitle->getRecord(cnt-1);
        if (m_pos < last->startTime || m_pos > last->endTime){
            MessageBeep(-1);
            qDebug() << "add record failed: m_pos=" << m_pos;
            return;
        }
        last->endTime = m_pos;
        rec->startTime = m_pos;
    } else {
        rec->startTime = m_session.getStartTime();
    }

    rec->endTime = m_length;
    rec->content = SrtRecord::sDefaultText;
    m_record = rec;
    if (subtitle->append(rec)){
        m_activeLine = cnt;
        qInfo() << "add record succeed.";
    }
}

void AddRecordCommand::undo()
{

}

void AddRecordCommand::redo()
{

}
