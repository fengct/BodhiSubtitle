#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "bodhiapp.h"
#include "bodhicore.h"
#include "srtarchive.h"
#include "parser.h"
#include "srtview.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    myApp(NULL)
{
    ui->setupUi(this);

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
}

void MainWindow::on_btnNewSubtitle_clicked()
{
    QString lastDir = tr("./");
    //QString filter = tr("MP3 文件(*.mp3)");
    QString filter = tr("SRT 文件(*.srt)");

    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), lastDir, filter);

    QString label = tr("new srt");
    SrtArchive archive(path);
    if (!archive.load()){
        qDebug() << "open srt file: " << path << " failed.";
        return;
    }
    SrtParser parser(archive);
    SrtRecord record(0);
    BodhiSubtitle *subtitle = new BodhiSubtitle();
    while (parser.parseOne(&record)){
        if (!subtitle->append(record)){
            qDebug() << "append record failed!";
            return;
        }
    }
    SrtView *view = new SrtView(this, this->ui, subtitle);
    int idx = ui->tabWidget->addTab(view, label);
    view->setTabIndex(idx);
    ui->tabWidget->tabBar()->setCurrentIndex(idx);
}

void MainWindow::on_btnProofReading_clicked()
{

}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    QWidget *tab = ui->tabWidget->widget(index);
    SrtView *sv = dynamic_cast<SrtView*>(tab);
    if (sv && sv->tabIndex() == index){
        sv->onTabActived();
    }
}
