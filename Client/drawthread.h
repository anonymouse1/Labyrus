#ifndef DRAWTHREAD_H
#define DRAWTHREAD_H

#include <QThread>
#include <drawgl.h>
#include <mainwindow.h>
#include <math.h>

class DrawGl;
class MainWindow;

const double speed = 0.01;

class DrawThread : public QThread
{
    Q_OBJECT
public:
    explicit DrawThread(DrawGl *wid, MainWindow *m, QObject *parent = 0);
private:
    void run();
    DrawGl *widget;
    QTimer *nextTimeTimer;
    MainWindow *main;

private slots:
    void nextTime();
};

#endif // DRAWTHREAD_H
