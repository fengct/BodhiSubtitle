#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QStandardItemModel>
#include <QModelIndex>
#include "config.h"

namespace Ui {
class MainWindow;
}
class BodhiApp;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void onStart(QApplication &app);
private slots:
    void on_btnNewSubtitle_clicked();

    void on_btnCollate_clicked();

    void on_btnPlay_clicked();

    void on_history_item_clicked(QModelIndex index);

    void on_tabWidget_currentChanged(int index);

    void on_actionClose_triggered();
    void on_actionSave_triggered();
private:
    void updateHistoryList();
    void createSession(Work &w);
private:
    Ui::MainWindow *ui;
    QStandardItemModel *m_historyFiles;
    BodhiApp *myApp;
    //just for test
    std::list<QString> m_contents;
    int m_currentTabIndex;
};

#endif // MAINWINDOW_H
