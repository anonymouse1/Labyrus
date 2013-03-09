#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QWidget>
#include <mainwindow.h>
#include <ui_startDialog.h>
#include <QDir>
#include <QFileInfoList>
#include <connection.h>
#include <QSettings>


const int dx[4]={0, 1, 0, -1};
const int dy[4]={1, 0, -1, 0};

#ifdef PORTABLE
    const QString skinPath = "../skins/";
    const QString settingsFile = "../labyrus.ini";
#else
    const QString skinPath = "/usr/share/labyrus/skins/";
    const QString settingsFile "~/.labyrus/labyrus.ini";
#endif


class startDialog : public QWidget
{
    Q_OBJECT
public:
    explicit startDialog(QApplication *a, int argc, char *argv[], QWidget *parent = 0);
    ~startDialog();

private:
    Ui::Form *ui;
    MainWindow *w;
    QPixmap *pix;
    Connection *connectWindow;
    QApplication *app;

    void scanSkins();
protected:
    void paintEvent(QPaintEvent *);
private slots:
    void saveSettings();
    void loadSettings();
signals:
    
public slots:
    void start();
    void setPix(QString);
};

#endif // STARTDIALOG_H
