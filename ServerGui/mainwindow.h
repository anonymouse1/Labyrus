#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QProcess>
#include <QTimer>
#include <QMainWindow>
#include <qmess
#include <QDebug>
#include <QSettings>
#include <QDir>
#include <console.h>
#include <QEventLoop>

namespace Ui {
    class MainWindow;
}

#ifdef PORTABLE
    const QString prefix = "./";
    const QString settingsFile = "../labyrus.ini";
#else
    const QString prefix = "/usr/bin/";
    const QString settingsFile = QDir::homePath() + "/.labyrus/labyrus.ini";
#endif


class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void start();
    void aboutQt();
    void about();
    void saveSettings();
    void loadSettings();
    void serverSaid();
    void checkForShutDown();
private:
    Ui::MainWindow *ui;
    QProcess *server;
    Console *console;
    QTimer *serverShutDown;
    QEventLoop *loop;
};

#endif // MAINWINDOW_H
