#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QTcpSocket>
#include <QHostAddress>
#include <QThread>
#include <QMessageBox>
#include <QTimer>
#include <QPoint>
#include <QPointF>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QTimeLine>
#include <QMutex>
#include <QTime>
#include <QEventLoop>
#include <QApplication>
#include <networkclass.h>
#include <drawgl.h>
#include <assert.h>
#include <stdlib.h>
#include <calculationthread.h>
#include <iostream>
#include <algorithm>
#include <math.h>

using std::swap;

class DrawGl;
class CalculationThread;

class MainWindow : public QObject
{
    Q_OBJECT
    
public:
    explicit MainWindow(QApplication *a, QHostAddress ip, quint16 port, QByteArray l, QString skin, QObject *parent = 0);
    ~MainWindow();

    void startBot();
    bool stopBot;


    bool w[100][100];

    CalculationThread *thread;
    QApplication *app;
    DrawGl *widget;
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);

   /* int lefter();
    int righter();*/
    int nap;//0 - up
            //1 - right
            //2 - down
            //3 -- left

    /*void taskKill();
    void fgup();
    void fgleft();
    void fgright();
    void fgdown();*/
private:
    void close();
    void processInformation();
    double scanInt();

    bool isWallUp(QPoint c);
    bool isWallDown(QPoint c);
    bool isWallLeft(QPoint c);
    bool isWallRight(QPoint c);

    /*bool noWall(int d);
    void createWall(int x, int y, int flag);
    void eraseWall(int x, int y, int flag);
    void invertNap();

    int backward();*/

    bool superDfs();
    void elementarMove(double x, double y);
    void syncNap(int);
    void standartMove(double x1, double y1, double x2, double y2);

    double fabs(double);

    QByteArray login;
    QTimeLine *startLine;
    QTimer *repaintTimer;
    NetworkClass *input;
    QPoint integerCoord;
    int backupPerspective;
    void sleep(int ms);
    QPoint getRealCoord();
    int getAngle(double x, double y, double x1, double y1);


signals:
    void successConnection();
    void fail();

private slots:
    void startTimerChanged(qreal);
    void startingFinished();
    void connectedSuccess();
    void connectionFailed();
    void gameStart();

public slots:
    void legalStop();
};

#endif // MAINWINDOW_H
