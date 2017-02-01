
#include "./h/charsetdetect.h"
#include "srtarchive.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "bodhicore.h"
#pragma execution_character_set("utf-8")

static QString LineSep = ("\r\n");

const char * getFileEncodeName(const char* path)
{
    FILE *fs = fopen(path, "rb");
    if (fs == NULL){
        return NULL;
    }
    char buf[4096] = {0};
    int sz = 0;
    csd_t csd = csd_open();
    sz = fread(buf, sizeof(buf), 1, fs);
    while (sz != 0){
        int result = csd_consider(csd, buf, sz);
        if (result < 0){
            return NULL;
        }else if(result > 0){
            break;
        }
    }

    const char *name = csd_close(csd);
    if (name)
        qDebug() << "file: [" << path << "] encode: " << name;
    else
        qWarning() << "file: [" << path << "] encode: unkown.";
    return name;
}

SrtArchive::SrtArchive(const QString &path)
    : m_path(path)
    , m_curLine(-1)
{

}

bool SrtArchive::load()
{
    QFile file(m_path);
    qInfo() << "open srt file: " << m_path;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qWarning() << "open file failed!";
        return false;
    }

    const char *encodeName = getFileEncodeName(m_path.toStdString().c_str());

    QTextStream instream(&file);
    instream.setAutoDetectUnicode(true);
    m_bom = instream.generateByteOrderMark();
    QString line;
    while (instream.readLineInto(&line))
        m_lines.push_back(line);
    return true;
}

bool SrtArchive::save()
{
    QFile file(m_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream ostream(&file);
    ostream.setCodec(("UTF-8"));
    ostream.setGenerateByteOrderMark(m_bom);

    QStringList::Iterator it = m_lines.begin();
    for (; it != m_lines.end(); it++){
        ostream << *it << LineSep;
    }
    ostream.flush();
    return true;
}

bool SrtArchive::write(const BodhiSubtitle &data, const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream ostream(&file);
    ostream.setCodec(("UTF-8"));
    ostream.setGenerateByteOrderMark(m_bom);

    int cnt = data.recordCount();
    for (int i = 0; i < cnt; i++){
        const SrtRecordPtr record = data.getRecord(i);
        ostream << record->sequence + 1 << LineSep;
        ostream << record->ts() << " -->" << record->te() << LineSep;
        ostream << record->content << LineSep;
        ostream << LineSep;
    }
    ostream.flush();
    return true;
}

//current line number
int SrtArchive::lineNumber() const
{
    return m_curLine;
}

int SrtArchive::totalLineNumber() const
{
    return m_lines.size();
}

const QString* SrtArchive::previousLine() const
{
    int idx = m_curLine - 1;
    if (idx > 0)
        return &m_lines.at(idx);
    else
        return (QString*)0;
}

const QString* SrtArchive::nextLine() const
{
    if (m_curLine < totalLineNumber())
        m_curLine++;

    if (m_curLine < totalLineNumber())
        return &m_lines.at(m_curLine);
    else
        return (QString*)0;
}
