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
#include <QString>
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
    explicit MainWindow(QApplication *a, QHostAddress ip, quint16 port, QByteArray l, QString skin, double mouseSensitivity, QObject *parent = 0);
    ~MainWindow();

    void startBot();
    bool stopBot;
    bool updateProgress();

    bool w[100][100][100];

    bool ctrlPressed;
    bool finished;
    CalculationThread *thread;
    QApplication *app;
    DrawGl *widget;
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);

private:
    void processInformation();
    double scanInt();
    void win();
    void restart();

    bool isWallUp(gpoint c);
    bool isWallDown(gpoint c);
    bool isWallLeft(gpoint c);
    bool isWallRight(gpoint c);
    bool isWallForward(gpoint c);
    bool isWallBackward(gpoint c);

    bool superDfs();
    void elementarMove(fpoint to);
    void syncNap(int, int);
    void standartMove(fpoint from, fpoint to);

    double fabs(double);

    bool progress[100][100][100];

    QByteArray login;
    QTimeLine *startLine;
    QTimer *repaintTimer;
    NetworkClass *input;
    gpoint integerCoord;
    QTimer *checkOrDie;
    int backupPerspective;
    void sleep(int ms);
    gpoint getRealCoord();
    int getAngle(double x, double y, double x1, double y1);
    int getYAngle(double rast, double h, double h1);
    fpoint genFPoint(double x, double y, double h);
    double sqr(double);


signals:
    void successConnection();
    void fail();

private slots:
    void startTimerChanged(qreal);
    void startingFinished();
    void connectedSuccess();
    void connectionFailed();
    void gameStart();
    void checkForDie();

public slots:
    void legalStop();
};

#endif // MAINWINDOW_H
