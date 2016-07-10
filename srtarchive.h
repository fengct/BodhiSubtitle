#ifndef SRTDOCUMENT_H
#define SRTDOCUMENT_H

#include <QString>
#include <QStringList>

class BodhiSubtitle;
class SrtArchive
{
    QString m_path;
    QStringList m_lines;
    mutable int m_curLine;
    bool m_bom;
public:
    SrtArchive(const QString &path);

    bool load();

    bool save();

    bool write(const BodhiSubtitle &data, const QString &path);

    //current line number
    int lineNumber() const;

    int totalLineNumber() const;

    const QString* previousLine() const;

    const QString* nextLine() const;
};

#endif // SRTDOCUMENT_H

