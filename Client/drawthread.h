#ifndef DRAWTHREAD_H
#define DRAWTHREAD_H

#include <QThread>
#include <drawgl.h>
#include <mainwindow.h>
#include <math.h>

class DrawGl;
class MainWindow;

const double speed = 0.01;
const int lacency = 100; // ms

class DrawThread : public QThread
{
    Q_OBJECT
public:
    explicit DrawThread(DrawGl *wid, MainWindow *m, QObject *parent = 0);
private:
    void run();
    DrawGl *widget;
    QTimer *nextTimeTimer, *serverRefresh;
    MainWindow *main;

    void check(double &dx, double &dy);

private slots:
    void nextTime();
    void refreshCoord();
};

#endif // DRAWTHREAD_H
