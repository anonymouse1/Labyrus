#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>
#include <QSettings>
#include <QDir>


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
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
