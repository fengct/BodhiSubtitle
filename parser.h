#ifndef PARSER_H
#define PARSER_H

#include <QObject>
#include "bodhicore.h"

class SrtArchive;
class SrtParser : public QObject
{
    const SrtArchive &m_archive;
    QString m_errorString;
    QString m_warningString;
    bool m_allowEmptyContent;
public:
    SrtParser(const SrtArchive &archive);

    bool parseOne(SrtRecord *record);

    const QString& getErrorString() const { return m_errorString; }

    const QString& getWarningString() const { return m_warningString; }
protected:
    bool parseTimestampLine(const QString &line, SrtRecord &record);
    bool parseTimestamp(const QString &timeStr, long &ms);
};

#endif // PARSER_H

