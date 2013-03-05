#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QWidget>
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
    explicit startDialog(QApplication *a, int argc, char *argv[], QWidget *parent = 0);

private:
    Ui::Form *ui;
    MainWindow *w;
    QPixmap *pix;
    Connection *connectWindow;
    QString skinPath;
    QApplication *app;
    QTimer *checkOrDie;

    void scanSkins();
protected:
    void paintEvent(QPaintEvent *);
signals:
    
public slots:
    void start();
    void setPix(QString);
private slots:
    void checkForDie();
};

#endif // STARTDIALOG_H
