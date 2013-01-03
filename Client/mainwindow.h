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
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QTimeLine>
#include <commandsend.h>
#include <drawgl.h>
#include <assert.h>
#include <stdlib.h>
#include <drawthread.h>
#include <iostream>
#include <algorithm>

using std::swap;

class DrawGl;
class DrawThread;

class MainWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit MainWindow(QApplication *a, QHostAddress ip, quint16 port, QByteArray l, QString skin, QWidget *parent = 0);
    ~MainWindow();

    void startBot();


    int walls[1000][3];
    bool w[100][100];
    int n, m;
    QPoint coord;
    int otherHeroes;
    QPoint heroes[100];
    int numberArsenals;
    QPoint arsenal[100];
    QString heroNames[100];
    int otherAlive[100];
    int patrons;
    int wall;
    int destroy;
    bool alive;
    QPoint hospital;


    QApplication *app;
    QTcpSocket *mainSocket;
    CommandSend *command;
    DrawGl *widget;
    void keyPressEvent(QKeyEvent *);

    int lefter();
    int righter();
    int nap;//0 - up
            //1 - right
            //2 - down
            //3 -- left

    void taskKill();
    void fgup();
    void fgleft();
    void fgright();
    void fgdown();



private:
    void close();
    int scanInt();

    int getRealX(double x);
    int getRealY(double y);

    bool isWallUp(QPoint c);
    bool isWallDown(QPoint c);
    bool isWallLeft(QPoint c);
    bool isWallRight(QPoint c);

    bool noWall(int d);
    void createWall(int x, int y, int flag);
    void eraseWall(int x, int y, int flag);
    void invertNap();

    int backward();

    void gameStart();
    bool superDfs();
    void strangeWait();
    void syncNap(int);
    void standartMove(bool);

    double fabs(double);

    QByteArray login;
    QTimer *animateTimer;
    QTimer *unScanN;
    QTimer *failConnection;
    QTimeLine *startLine;
    DrawThread *thread;


    bool fullRefresh;
    bool scanN;

    int myDescriptor;
    int descriptors[100];

signals:
    void successConnection();
    void fail();

private slots:
    void connectionEstablished();
    void readField();
    void readHeroes();
    void readInformation();
    void seekNet();
    void setFullRefresh();
    void setUnScan();
    void startTimerChanged(qreal);
    void startingFinished();
    void animate();
    void connectionFailed();
};

#endif // MAINWINDOW_H
