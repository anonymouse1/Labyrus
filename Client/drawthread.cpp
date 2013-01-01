#include "drawthread.h"

DrawThread::DrawThread(DrawGl *wid, QObject *parent) :
    QThread(parent)
{
    widget = wid;
}

void DrawThread::run() {
    qDebug() << "run called";
    repaintTimer = new QTimer;

    QObject::connect(repaintTimer, SIGNAL(timeout()), widget, SLOT(update()));

    repaintTimer->setInterval(1);
    repaintTimer->start();

    exec();
}

void DrawThread::repaintTime() {
    widget->repaint();
}

