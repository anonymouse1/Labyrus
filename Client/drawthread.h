#ifndef DRAWTHREAD_H
#define DRAWTHREAD_H

#include <QThread>
#include <drawgl.h>

class DrawGl;

class DrawThread : public QThread
{
    Q_OBJECT
public:
    explicit DrawThread(DrawGl *wid, QObject *parent = 0);
private:
    void run();
    DrawGl *widget;
    QTimer *repaintTimer;

private slots:
    void repaintTime();
};

#endif // DRAWTHREAD_H
