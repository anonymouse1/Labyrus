#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QWidget>
#include <commandsend.h>
#include <mainwindow.h>
#include <ui_startDialog.h>
#include <QDir>
#include <QFileInfoList>
#include <connection.h>


const int dx[4]={0, 1, 0, -1};
const int dy[4]={1, 0, -1, 0};

class startDialog : public QWidget
{
    Q_OBJECT
public:
    explicit startDialog(QWidget *parent = 0);

    QApplication *app;
private:
    Ui::Form *ui;
    MainWindow *w;
    CommandSend *command;
    QPixmap *pix;
    Connection *connectWindow;

    void scanSkins();
protected:
    void paintEvent(QPaintEvent *);
signals:
    
public slots:
    void start();
    void setPix(QString);
};

#endif // STARTDIALOG_H
