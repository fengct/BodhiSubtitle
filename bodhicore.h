#ifndef BODICORE_H
#define BODICORE_H

#include <QStringList>
#include <QList>
#include <memory>

class SrtRecord
{
public:
    SrtRecord(int seq, long stime = 0, long etime = 0, const QString &cont = sDefaultText);

    //zero based internal, but display and output is start with 1.
    int sequence;
    qint64 startTime;
    qint64 endTime;
    QString content;
    static QString sDefaultText;

public:
    QString ts() const;
    QString te() const;
};
typedef std::shared_ptr<SrtRecord> SrtRecordPtr;

class BodhiSubtitle
{
    QList<SrtRecordPtr> m_data;
    bool m_changed;
    int m_minTimeLength;
public:
    BodhiSubtitle();

    const SrtRecordPtr getRecord(int sequence) const;

    int recordCount() const;

    void resetChangeFlag() { m_changed = false; }
    bool changed() const { return m_changed; }

    bool modifyRecord(int sequence, SrtRecordPtr newValue);

    bool append(const SrtRecordPtr record);

    //return record count after remove operation.
    //return -1 if sequence is invalid.
    int remove(int sequence);
    //timeOffset is related to the startTime of the record.
    bool split(int sequence, long timeOffset);
    bool merge(int startSequence, int endSequence);
    bool adjustTimePoint(int sequence, long timeDelta);

    bool checkTimestamp(int sequence, const SrtRecordPtr newValue);
protected:
    void updateSequence(int start, int delta);
};

#endif // BODICORE_H

