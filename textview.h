#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include <QWidget>
#include <QTextEdit>

class TextView : public QWidget
{
    Q_OBJECT
public:
    explicit TextView(QWidget *parent);

//    QString label() const;

    void setTabIndex(int idx);

    void showTextContent(QString content);

    void showTextFile(QString path);
signals:

public slots:

private:
    QTextEdit *m_edit;
    QString m_textFilePath;
    int m_tabIndex;
};

#endif // TEXTVIEW_H
