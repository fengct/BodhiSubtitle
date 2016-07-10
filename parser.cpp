#include "parser.h"
#include "srtarchive.h"
#include <QTime>
#include <QDebug>
#include <QRegExp>
#pragma execution_character_set("utf-8")

SrtParser::SrtParser(const SrtArchive &archive)
    : m_archive(archive)
    , m_allowEmptyContent(false)
{
}

 bool SrtParser::parseOne(SrtRecord* record)
{
    Q_ASSERT(record);

    SrtRecord rec(0);
    const QString *line;
    do {
        line = m_archive.nextLine();
        if (!line)
            return false;

        if (line->trimmed().isEmpty())
            continue;
        else
            break;
    }while(true);

    bool ok = true;
    int seq = line->toUInt(&ok);
    if (!ok){
        m_errorString = QString("第%1行：期望得到序号。").arg(m_archive.lineNumber());
        return false;
    }
    rec.sequence = seq-1;

    line = m_archive.nextLine();
    if (!line){
        m_errorString = QString("读取时间戳失败.");
        return false;
    }
    if (!parseTimestampLine(*line, rec)){
        m_errorString = QString("第%1行：解析时间戳失败: %2").arg(m_archive.lineNumber()).arg(m_errorString);
        return false;
    }

    QStringList content;
    do{
        line = m_archive.nextLine();
        if (!line)
            break;
        if (line->isEmpty()){
            if (content.isEmpty() && m_allowEmptyContent){
                content.append(*line);
                m_warningString = QString("第%1行：内容为空。").arg(m_archive.lineNumber());
            }
            break;
        }
        content.append(*line);
    }while(true);
    if (content.isEmpty()){
        m_errorString = QString("第%1行：未读到字幕文本。").arg(m_archive.lineNumber());
        return false;
    }
    rec.content = content.join("\r\n");
    *record = rec;

    return true;
}

bool SrtParser::parseTimestampLine(const QString &line, SrtRecord &record)
{
    QString sep = tr("-->");
    QStringList times = line.split(sep);
    if (times.size() != 2){
        m_errorString = QString("第%1行：解析时间失败，请确定时间范围由-->分隔。").arg(m_archive.lineNumber());
        return false;
    }
    long ms;
    if (!parseTimestamp(times[0], ms)){
        m_errorString = QString("第%1行：解析开始时间失败。").arg(m_archive.lineNumber());
        return false;
    }
    record.startTime = ms;
    if (!parseTimestamp(times[1], ms)){
        m_errorString = QString("第%1行：解析结束时间失败。").arg(m_archive.lineNumber());
        return false;
    }
    record.endTime = ms;
    return true;
}

bool SrtParser::parseTimestamp(const QString &timeStr, long &ms)
{
    qDebug() << QString("time contents=<<%1>>").arg(timeStr);
    QRegExp re("\\d\\d:\\d\\d:\\d\\d,\\d\\d\\d");
    if (!timeStr.contains(re))
        return false;
    QTime time = QTime::fromString(timeStr.trimmed(), ("hh:mm:ss,zzz"));
    ms = (time.hour()*3600 + time.minute()*60 +time.second())*1000 + time.msec();
    return true;
}
