#ifndef CALCULATIONTHREAD_H
#define CALCULATIONTHREAD_H

#include <QThread>
#include <drawgl.h>
#include <mainwindow.h>
#include <math.h>

class DrawGl;
class MainWindow;

const double speed = 0.012;
const double radiusOfPlayer = 0.3;

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
    bool shiftPressed;

    int currentTime;
private:
    void run();
    DrawGl *widget;
    QTimer *nextTimeTimer, *serverRefresh;
    NetworkClass *main;

    void check(double &dx, double &dy, double &dh);
    bool equal(QPointF, QPointF);
    void checkForWall(double &dx, double &dy, double x1, double y1, double x2, double y2);
    void checkForDhWall(double &dh, double x, double y, double h, double x1, double y1);
    bool heightEqualToMe(double height);
    double max(double a, double b);

private slots:
    void nextTime();
    void refreshCoord();
};

#endif // CALCULATIONTHREAD_H
