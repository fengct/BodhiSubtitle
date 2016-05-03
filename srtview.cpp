#include <math.h>
#include "srtview.h"
#include "bodhicore.h"
#include <QPainter>
#include <QColor>
#include <QBrush>
#include <QDebug>
#include <QMouseEvent>
#include <QKeyEvent>

//static const int LH = 40;
//static const int PH = 10*LH;
static const int SB_WIDTH = 20;
static const int SEQ_WIDTH = 50;
static const int TS_WIDTH = 120;
static const int CONTENT_WIDTH = 300;
static const int TABLE_WIDTH = SEQ_WIDTH + TS_WIDTH*2 + CONTENT_WIDTH;

void MyScrollBar::onWheelEvent(QWheelEvent *event)
{
    QScrollBar::wheelEvent(event);
    qDebug() << "wheel event fired on scroll bar.";
}

SrtView::SrtView(QWidget *parent, Ui::MainWindow *ui, BodhiSubtitle *subtitle)
    : QWidget(parent)
    , m_ui(ui)
    , m_subtitle(subtitle)
    , m_marginLT(20, 20)
    , m_marginRB(20, 20)
    , m_scrollBar(0)
    , m_LH(40)
    , m_PH(10*m_LH)
    , m_tsPos(m_marginLT.x() + 50)
    , m_tePos(m_tsPos + 120)
    , m_contentPos(m_tePos + 120)
    , m_tabIndex(0)
    , m_playingIndex(-1)
    , m_selectStart(-1)
    , m_selectCount(0)
{

}

void SrtView::setTabIndex(int idx)
{
    m_tabIndex = idx;
}

void SrtView::onTabActived()
{
    if (!m_scrollBar){
        m_scrollBar = new QScrollBar(Qt::Vertical, this);
        connect(this, SIGNAL(wheelEventFired(QWheelEvent*)), m_scrollBar, SLOT(onWheelEvent(QWheelEvent*)));
    }

    QRect viewPort = this->geometry();
    QRect sbArea = viewPort;
    sbArea.setLeft(viewPort.width() - SB_WIDTH);
    m_scrollBar->setGeometry(sbArea);
    m_scrollBar->setMinimum(0);
    m_PH = height() - height() % m_LH;
    int max = m_marginLT.y() + m_LH * m_subtitle->recordCount();
    max = ((max /*+ height()/2*/) / m_PH) * m_PH;
    m_scrollBar->setMaximum(max);
    m_scrollBar->setSingleStep(m_LH);
    m_scrollBar->setPageStep(m_PH); //this->height() - m_marginLT.y() - m_marginRB.y());
    m_scrollBar->setTracking(true);
    m_scrollBar->setVisible(true);
    QObject::connect(m_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(onScrolled(int)));
}

void SrtView::onScrolled(int pos)
{
    update();
}

void SrtView::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    int pos = m_scrollBar->value();
    painter.translate(0, -pos);
    int startSeq = (pos - m_marginLT.y())/m_LH;
    int endSeq = startSeq + height()/m_LH;
    endSeq = std::min(endSeq, m_subtitle->recordCount()-1);
    QColor color1(0xdddddd), color2(0xcccccc), colorSel(0x2244ff);
    QRect rect;
    for (int seq = startSeq; seq <= endSeq; seq++){
        const SrtRecord *record = m_subtitle->getRecord(seq);
        int y = m_marginLT.y() + record->sequence * m_LH;
        rect.setLeft(m_marginLT.x());
        rect.setWidth(SEQ_WIDTH + 2*TS_WIDTH + CONTENT_WIDTH);
        rect.setTop(y);
        rect.setHeight(m_LH);
        if (seq % 2)
            painter.fillRect(rect, color1);
        else
            painter.fillRect(rect, color2);
        if (m_selectCount > 0){
            int dseq = record->sequence - m_selectStart;
            if (dseq >= 0 && dseq < m_selectCount){
                painter.fillRect(rect, colorSel);
            }
        }
        QString text;
        text.setNum(record->sequence + 1); //display start from 1.
        rect.setRight(m_marginLT.x() + SEQ_WIDTH);
        painter.drawText(rect, Qt::AlignCenter | Qt::AlignVCenter, text);
        rect.setLeft(rect.right());
        rect.setWidth(TS_WIDTH);
        painter.drawText(rect, Qt::AlignCenter | Qt::AlignVCenter, record->ts());
        rect.setLeft(rect.right());
        rect.setWidth(TS_WIDTH);
        painter.drawText(rect, Qt::AlignCenter | Qt::AlignVCenter, record->te());
        rect.setLeft(rect.right() + 20);
        rect.setWidth(CONTENT_WIDTH);
        painter.drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, record->content);
    }
}

void SrtView::mousePressEvent(QMouseEvent * event)
{
    QWidget::mousePressEvent(event);
    int sel = pickRecord(event->x(), event->y() + m_scrollBar->value());
    if (sel > 0){
        m_selectStart = sel;
        m_selectCount = sel >= 0 ? 1 : 0;
        update();
    }
}

int SrtView::pickRecord(int x, int y)
{
    int dx = x - m_marginLT.x(),
        dy = y - m_marginLT.y();
    if (dx < 0 || dx > TABLE_WIDTH)
        return -1;
    if (dy < 0 || dy > m_LH * m_subtitle->recordCount())
        return -1;
    int seq = dy/m_LH;
    qDebug() << "mouse pressed: x=" << x << ", y=" << y << ", seq=" << seq;
    return seq;
}

void SrtView::wheelEvent(QWheelEvent *event)
{
    QWidget::wheelEvent(event);
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull()) {
        //scrollWithPixels(numPixels);
        //scroll(numPixels.rx(), numPixels.ry());
    } else if (!numDegrees.isNull()) {
        //QPoint numSteps = numDegrees / 15;
        //scrollWithDegrees(numSteps);
    }

    event->accept();
    qDebug() << "wheel event fired: x=" << event->x() << ", y=" << event->y();
    //emit wheelEventFired(event);
}

int SrtView::shiftSelectItem(int offset)
{
    if (m_selectCount <= 1){
        int old = m_selectStart;
        m_selectStart += offset;
        if (m_selectStart < 0 || m_selectStart >= m_subtitle->recordCount()){
            m_selectStart = old;
        } else {
            m_selectCount = 1;
            update();
        }
        return m_selectStart;
    } else {
        return -1;
    }
}

int SrtView::singleSelectItem() const
{
    if (m_selectCount == 1)
        return m_selectStart;
    return -1;
}

void SrtView::scroll(int offset)
{
    int pos = m_scrollBar->value();
    m_scrollBar->setValue(pos + offset);
}

void SrtView::scrollTo(int pos)
{
    m_scrollBar->setValue(pos);
}

bool SrtView::inVisibleArea(int sequence)
{
    qDebug() << "scroll pos=" << m_scrollBar->value();
    if (m_marginLT.y() + sequence * m_LH < m_scrollBar->value())
        return false;
    if (m_marginLT.y() + (sequence + 1) * m_LH > m_scrollBar->value() + geometry().height())
        return false;
    return true;
}

void SrtView::keyPressEvent(QKeyEvent * event)
{
    int offset = 0;
    bool ctrlHold = event->modifiers() & Qt::ControlModifier;
    switch(event->key()){
    case Qt::Key_Up:
        if (ctrlHold){
            scroll(-m_scrollBar->singleStep());
        }else{
            int seq = singleSelectItem();
            if (seq > 0 && !inVisibleArea(seq)){
                //let it show the select item.
                scrollTo(((m_marginLT.y() + (seq + 1) * m_LH ) / m_PH) * m_PH);
            }else{
                seq = shiftSelectItem(-1);
                if (seq >= 0 && !inVisibleArea(seq)){
                    scroll(-m_scrollBar->singleStep());
                }
            }
        }
        break;
    case Qt::Key_Down:
        if (ctrlHold){
            scroll(m_scrollBar->singleStep());
        }else{
            int seq = singleSelectItem();
            if (seq > 0 && !inVisibleArea(seq)){
                scrollTo(((m_marginLT.y() + (seq + 1) * m_LH ) / m_PH) * m_PH);
            }else{
                seq = shiftSelectItem(1);
                if (seq >= 0 && !inVisibleArea(seq)){
                    scroll(m_scrollBar->singleStep());
                }
            }
        }
        break;
    case Qt::Key_PageUp:
        scroll(-m_scrollBar->pageStep());
        break;
    case Qt::Key_PageDown:
        scroll(m_scrollBar->pageStep());
        break;
    case Qt::Key_Home:
        if (ctrlHold)
            scrollTo(0);
        else{
            int seq = singleSelectItem();
            if (seq > 0 && !inVisibleArea(seq)){
                //scroll to the page the item in
                int page = (m_marginLT.y() + (seq + 1) * m_LH) / m_PH;
                scrollTo(page * m_PH);
            }
        }
        break;
    case Qt::Key_End:
        if (ctrlHold)
            scrollTo(m_scrollBar->maximum());
        break;
    }
}
