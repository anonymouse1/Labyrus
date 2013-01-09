#ifndef CALCULATIONTHREAD_H
#define CALCULATIONTHREAD_H

#include <QThread>
#include <drawgl.h>
#include <mainwindow.h>
#include <math.h>

class DrawGl;
class MainWindow;

const double speed = 0.01;

class CalculationThread : public QThread
{
    Q_OBJECT
public:
    explicit CalculationThread(DrawGl *wid, NetworkClass *input);

    bool upPressed;
    bool leftPressed;
    bool rightPressed;
    bool downPressed;
    bool leftStrife;
    bool rightStrife;
    bool lookingUp;
    bool lookingDown;
private:
    void run();
    DrawGl *widget;
    QTimer *nextTimeTimer, *serverRefresh;
    NetworkClass *main;

    void check(double &dx, double &dy);
    bool equal(QPointF, QPointF);
    void checkForWall(double &dx, double &dy, double x1, double y1, double x2, double y2);

private slots:
    void nextTime();
    void refreshCoord();
};

#endif // CALCULATIONTHREAD_H
