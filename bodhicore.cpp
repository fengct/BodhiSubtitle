#include "bodhicore.h"
#include <QTime>
#include <QString>
#pragma execution_character_set("utf-8")

QString SrtRecord::sDefaultText = "双击以编辑";

SrtRecord::SrtRecord(int seq, long stime, long etime, const QString &cont)
    : sequence(seq)
    , startTime(stime)
    , endTime(etime)
    , content(cont)
{

}

QString ms2string(long ms)
{
    QString s;
    int sec = ms / 1000;
    ms %= 1000;
    int h = sec / 3600;
    sec %= 3600;
    int m = sec / 60;
    sec %= 60;
    QTime t(h, m, sec, ms);
    return t.toString("hh:mm:ss,zzz");
}

QString SrtRecord::ts() const
{
    return ms2string(startTime);
}

QString SrtRecord::te() const
{
    return ms2string(endTime);
}

BodhiSubtitle::BodhiSubtitle()
    : m_changed(false)
    , m_minTimeLength(3000)
{

}

const SrtRecord* BodhiSubtitle::getRecord(int sequence) const
{
    if (sequence < 0)
        sequence = m_data.size() + (sequence % m_data.size());
    return &m_data.at(sequence);
}

int BodhiSubtitle::recordCount() const
{
    return m_data.size();
}

bool BodhiSubtitle::modifyRecord(int sequence, SrtRecord &newValue)
{
    Q_ASSERT(newValue.sequence == sequence);
    if (sequence < 0 || m_data.size() >= sequence)
        return false;

    if (!checkTimestamp(sequence, newValue))
        return false;

    m_data[sequence] = newValue;
    m_changed = true;
    return true;
}

bool BodhiSubtitle::append(const SrtRecord &record)
{
    if (record.sequence != m_data.size())
        return false;
    if (!checkTimestamp(record.sequence, record))
        return false;

    m_data.append(record);
    m_changed = true;
    return true;
}

//return record count after remove operation.
//return -1 if sequence is invalid.
int BodhiSubtitle::remove(int sequence)
{
    if (sequence < 0)
        sequence = m_data.size() + (sequence % m_data.size());

    updateSequence(sequence, -1);
    m_data.removeAt(sequence);
    m_changed = true;
    return m_data.size();
}

void BodhiSubtitle::updateSequence(int start, int delta)
{
    int seq = start;
    for(; seq < m_data.size(); seq++){
        SrtRecord &record = m_data[seq];
        record.sequence += delta;
    }
}

bool BodhiSubtitle::split(int sequence, long timeOffset)
{
    if (timeOffset < m_minTimeLength)
        return false;

    SrtRecord *record = const_cast<SrtRecord*>(getRecord(sequence));
    Q_ASSERT(record);
    if (record->endTime - timeOffset < m_minTimeLength)
        return false;

    SrtRecord newRecord(sequence+1, record->startTime + timeOffset, record->endTime);
    record->endTime = newRecord.startTime;
    updateSequence(newRecord.sequence, 1);
    m_data.insert(newRecord.sequence, newRecord);
    m_changed = true;
    return true;
}

bool BodhiSubtitle::merge(int startSequence, int endSequence)
{
    return false;
}

bool BodhiSubtitle::adjustTimePoint(int sequence, long timeDelta)
{
    return false;
}

bool BodhiSubtitle::checkTimestamp(int sequence, const SrtRecord &newValue)
{
    return true;
}
