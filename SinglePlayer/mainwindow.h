#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>
#include <QSettings>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    QString prefix;
    QString settingsFile;
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
