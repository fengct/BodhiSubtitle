#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "bodhiapp.h"
#include "bodhicore.h"
#include "srtarchive.h"
#include "parser.h"
#include "srtview.h"
#include <QFileDialog>
#include "bodhisession.h"
#include "bodhiplayer.h"
#include "config.h"
#include <QQuickWidget>
#include <QQmlContext>
#include <QCommandLinkButton>
#include <QStandardItem>
#include <QVariant>
#include <QJsonObject>
#include "config.h"
#pragma execution_character_set("utf-8")

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    myApp(NULL),
    m_historyFiles(NULL)
{
    ui->setupUi(this);
//    ui->quickWidget->setSource(QUrl::fromLocalFile("F:\\Data\\Code\\BodhiSubtitle\\test.qml"));
//    ui->quickWidget->rootContext()->setContextProperty("test", "some random text");
}

MainWindow::~MainWindow()
{
    delete ui;
    myApp->onAppStop();
    delete myApp;
}

void MainWindow::onStart(QApplication &app)
{
    myApp = new BodhiApp(*this, app);
    myApp->onAppStart();

    m_historyFiles = new QStandardItemModel(this);
    updateHistoryList();

    ui->listView->setModel(m_historyFiles);
    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_history_item_clicked(QModelIndex)));
    connect(ui->pushButton_playPause, SIGNAL(clicked()), this, SLOT(on_btnPlay_clicked()));

    ui->pushButton_playPause->setEnabled(false);
    ui->progressBar->setMaximum(0);
    ui->progressBar->setMinimum(0);
//    ui->progressBar->setTextVisible(false);
    ui->progressBar->setEnabled(false);
    ui->progressBar->setValue(0);
    ui->pushButton_playPause->setVisible(false);
    ui->progressBar->setVisible(false);

//    connect(ui->btnUndo, SIGNAL(clicked()), this, SLOT(on_btnUndo_cliccked()));
//    connect(ui->btnRedo, SIGNAL(clicked()), this, SLOT(on_btnRedo_cliccked()));
//    connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(on_btnAdd_cliccked()));
//    connect(ui->btnRemove, SIGNAL(clicked()), this, SLOT(on_btnRemove_cliccked()));
//    connect(ui->btnSplit, SIGNAL(clicked()), this, SLOT(on_btnSplit_cliccked()));
//    connect(ui->btnMerge, SIGNAL(clicked()), this, SLOT(on_btnMerge_cliccked()));
//    connect(ui->btnTimeDecrease, SIGNAL(clicked()), this, SLOT(on_btnTimeDec_clicked()));
//    connect(ui->btnTimeIncrease, SIGNAL(clicked()), this, SLOT(on_btnTimeInc_clicked()));
}

void MainWindow::updateHistoryList()
{
    m_historyFiles->clear();
    QFileInfo fileInfo;
    const WorkList &wlist = myApp->getConfig().getHistory();
    WorkList::const_iterator it=wlist.begin();
    for(; it != wlist.end(); it++){
        const Work &work = *it;
        QJsonObject jsonObj;
        Work2Json(work, &jsonObj);
        fileInfo.setFile(work.soundFile);
        QStandardItem *item = new QStandardItem(fileInfo.fileName());
        item->setData(QVariant(jsonObj));
        m_historyFiles->appendRow(item);
    }
}

void MainWindow::createSession(Work &work)
{
    if (myApp->fileIsOpen(work.subtitleFile)){
        qInfo() << "file: " << work.subtitleFile << " already open!";
        return;
    }

    BodhiSession *session = myApp->createSession(work);
    if (session){
        SrtView *view = new SrtView(this, this->ui, session->subtitle(), session);
        int idx = ui->tabWidget->addTab(view, session->label());
        view->setTabIndex(idx);
        session->setView(view);
//        myApp->getConfig().addHistory(session->getWorkInfo());
//        myApp->getConfig().saveHistory();

        connect(ui->btnUndo, SIGNAL(clicked()), session, SLOT(on_btnUndo_cliccked()));
        connect(ui->btnRedo, SIGNAL(clicked()), session, SLOT(on_btnRedo_cliccked()));
        connect(ui->btnAdd, SIGNAL(clicked()), session, SLOT(on_btnAdd_cliccked()));
        connect(ui->btnRemove, SIGNAL(clicked()), session, SLOT(on_btnRemove_cliccked()));
        connect(ui->btnSplit, SIGNAL(clicked()), session, SLOT(on_btnSplit_cliccked()));
        connect(ui->btnMerge, SIGNAL(clicked()), session, SLOT(on_btnMerge_cliccked()));
        connect(ui->btnTimeDecrease, SIGNAL(clicked()), session, SLOT(on_btnTimeDec_clicked()));
        connect(ui->btnTimeIncrease, SIGNAL(clicked()), session, SLOT(on_btnTimeInc_clicked()));

        ui->tabWidget->tabBar()->setCurrentIndex(idx);
    }
}

void MainWindow::on_history_item_clicked(QModelIndex index)
{
    qDebug() << "on_history_item_clicked";
    QStandardItem *item = m_historyFiles->itemFromIndex(index);
    QJsonObject obj = item->data().toJsonObject();
    Work w;
    if (!WorkFromJson(obj, &w)){
        qWarning() << "get work history failed!";
        return;
    }
    createSession(w);
}

void MainWindow::on_btnNewSubtitle_clicked()
{
    Config &config = myApp->getConfig();
    QString lastDir = config.getWorkDir();
    qDebug() << "lastDir = " << lastDir;
    QString mediaFilter = tr("MP3 文件(*.mp3)");
    QString srtFilter = tr("SRT 文件(*.srt)");

    QString mediaPath = QFileDialog::getOpenFileName(this, tr("Open File"), lastDir, mediaFilter);
    if (mediaPath.isEmpty())
        return;

    QString srtPath; // = getMatchedPath(mediaPath);
    if (ui->cbBindSrtFile->isChecked()){
        QFileInfo fi(srtPath);
        if (!fi.exists()){
            srtPath = QFileDialog::getOpenFileName(this, tr("Open File"), srtPath, srtFilter);
            if (srtPath.isEmpty())
                return;
        }
    }

    Work work;
    work.soundFile = mediaPath;
    work.subtitleFile = srtPath;
    work.type = WT_DICATION;
    createSession(work);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    qDebug() << "current tab changed to: " << index;
    ui->pushButton_playPause->setEnabled(index != 0);
    ui->progressBar->setEnabled(index != 0);
    ui->pushButton_playPause->setVisible(index != 0);
    ui->progressBar->setVisible(index != 0);
    if (index == 0){
        ui->progressBar->setValue(0);
        myApp->setActiveSession(NULL);
    } else {
        QWidget *tab = ui->tabWidget->widget(index);
        SrtView *sv = dynamic_cast<SrtView*>(tab);
        if (sv)
            myApp->setActiveSession(sv->session());
            if (sv->tabIndex() == index){
                sv->onTabActived();
        }
    }
}

void MainWindow::on_actionClose_triggered()
{
    BodhiSession *session = myApp->activeSession();
    if (session){
        disconnect(ui->btnUndo, SIGNAL(clicked()), session, SLOT(on_btnUndo_cliccked()));
        disconnect(ui->btnRedo, SIGNAL(clicked()), session, SLOT(on_btnRedo_cliccked()));
        disconnect(ui->btnAdd, SIGNAL(clicked()), session, SLOT(on_btnAdd_cliccked()));
        disconnect(ui->btnRemove, SIGNAL(clicked()), session, SLOT(on_btnRemove_cliccked()));
        disconnect(ui->btnSplit, SIGNAL(clicked()), session, SLOT(on_btnSplit_cliccked()));
        disconnect(ui->btnMerge, SIGNAL(clicked()), session, SLOT(on_btnMerge_cliccked()));
        disconnect(ui->btnTimeDecrease, SIGNAL(clicked()), session, SLOT(on_btnTimeDec_clicked()));
        disconnect(ui->btnTimeIncrease, SIGNAL(clicked()), session, SLOT(on_btnTimeInc_clicked()));

        int tabIndex = session->view()->tabIndex();
        ui->tabWidget->removeTab(tabIndex);
        myApp->closeSession(session);

        updateHistoryList();
    }
}

void MainWindow::on_btnCollate_clicked()
{
    qDebug("on_btnCollate_clicked");
}

void MainWindow::on_btnPlay_clicked()
{
    qDebug("on_btnPlay_clicked");
    BodhiSession *session = myApp->activeSession();
    if (session && session->player()){
        session->player()->playPause();
    }
}
