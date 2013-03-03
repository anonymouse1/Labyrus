#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    QString prefix;
    ~MainWindow();
private slots:
    void start();
    void aboutQt();
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
