#include "srtarchive.h"
#include <QFile>
#include <QTextStream>

static QString LineSep = ("\r\n");

SrtArchive::SrtArchive(const QString &path)
    : m_path(path)
    , m_curLine(-1)
{

}

bool SrtArchive::load()
{
    QFile file(m_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

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
