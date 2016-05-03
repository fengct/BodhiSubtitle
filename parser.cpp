#include "parser.h"
#include "srtarchive.h"
#include <QTime>

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
        //m_errorString.sprintf(tr("第%d行：期望得到序号。"), m_archive.lineNumber());
        return false;
    }
    rec.sequence = seq-1;

    line = m_archive.nextLine();
    if (!line){
        //m_errorString.sprintf(tr("读取时间戳失败。"));
        return false;
    }
    if (!parseTimestampLine(*line, rec)){
        //m_errorString.sprintf(tr("第%d行：解析时间戳失败。"), m_archive.lineNumber());
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
                //m_warningString.sprintf(tr("第%d行：内容为空。"), m_archive.lineNumber());
            }
            break;
        }
        content.append(*line);
    }while(true);
    if (content.isEmpty()){
        //m_errorString.sprintf(tr("第%d行：未读到字幕文本。"), m_archive.lineNumber());
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
        //m_errorString.sprintf(tr("第%d行：解析时间失败，请确定时间范围由-->分隔。"), m_archive.lineNumber());
        return false;
    }
    long ms;
    if (!parseTimestamp(times[0], ms)){
        //m_errorString.sprintf(tr("第%d行：解析开始时间失败。"), m_archive.lineNumber());
        return false;
    }
    record.startTime = ms;
    if (!parseTimestamp(times[1], ms)){
        //m_errorString.sprintf(tr("第%d行：解析结束时间失败。"), m_archive.lineNumber());
        return false;
    }
    record.endTime = ms;
    return true;
}

bool SrtParser::parseTimestamp(const QString &timeStr, long &ms)
{
    QTime time = QTime::fromString(timeStr.trimmed(), tr("hh:mm:ss,zzz"));
    ms = (time.hour()*3600 + time.minute()*60 +time.second())*1000 + time.msec();
    return true;
}
