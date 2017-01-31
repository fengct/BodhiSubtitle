#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QWidget>
#include "textview.h"

TextView::TextView(QWidget *parent) : QWidget(parent)
  ,m_edit(NULL)
  ,m_tabIndex(-1)
{
}

void TextView::setTabIndex(int idx)
{
    m_tabIndex = idx;
}

void TextView::showTextContent(QString content)
{
    if (!m_edit){
        m_edit = new QTextEdit(this);
        QWidget *widget = dynamic_cast<QWidget*>(parent());
        if (widget){
            QRect rect = widget->rect();
            qDebug() << "parent width: " << rect.width() << ", height: " << rect.height();
//            m_edit->move(20, 20);
            m_edit->resize(rect.width(), rect.height());
        }
    }
    m_edit->setText(content);
}

void TextView::showTextFile(QString path)
{
    QFile file(path);
    m_textFilePath = path;
    qInfo() << "load text file: " << path;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qWarning() << "open file failed!";
        return;
    }

    QTextStream instream(&file);
    instream.setAutoDetectUnicode(true);
    QString content = instream.readAll();
    showTextContent(content);
}
