#ifndef BODICORE_H
#define BODICORE_H

#include <QStringList>
#include <QList>

class SrtRecord
{
public:
    SrtRecord(int seq, long stime = 0, long etime = 0, const QString &cont = sDefaultText);

    int sequence;
    qint64 startTime;
    qint64 endTime;
    QString content;
    static QString sDefaultText;

public:
    QString ts() const;
    QString te() const;
};

class BodhiSubtitle
{
    QList<SrtRecord> m_data;
    bool m_changed;
    int m_minTimeLength;
public:
    BodhiSubtitle();

    const SrtRecord* getRecord(int sequence) const;

    int recordCount() const;

    void resetChangeFlag() { m_changed = false; }
    bool changed() const { return m_changed; }

    bool modifyRecord(int sequence, SrtRecord &newValue);

    bool append(const SrtRecord &record);

    //return record count after remove operation.
    //return -1 if sequence is invalid.
    int remove(int sequence);
    //timeOffset is related to the startTime of the record.
    bool split(int sequence, long timeOffset);
    bool merge(int startSequence, int endSequence);
    bool adjustTimePoint(int sequence, long timeDelta);

    bool checkTimestamp(int sequence, const SrtRecord &newValue);
protected:
    void updateSequence(int start, int delta);
};

#endif // BODICORE_H

