#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>

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

    void on_btnProofReading_clicked();

    void on_tabWidget_currentChanged(int index);

private:
    Ui::MainWindow *ui;
    BodhiApp *myApp;
    //just for test
    std::list<QString> m_contents;
};

#endif // MAINWINDOW_H
