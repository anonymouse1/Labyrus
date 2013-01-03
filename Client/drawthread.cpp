#include "drawthread.h"

DrawThread::DrawThread(DrawGl *wid, MainWindow *m, QObject *parent) :
    QThread(parent)
{
    widget = wid;
    main = m;
}

void DrawThread::run() {
    qDebug() << "run called";
    nextTimeTimer = new QTimer;

    QObject::connect(nextTimeTimer, SIGNAL(timeout()), this, SLOT(nextTime()));

    nextTimeTimer->setInterval(10);
    nextTimeTimer->start();

    exec();
}

void DrawThread::nextTime() {
    if (main->upPressed) {
        double deltaX = cos((-main->angle + 90) * M_PI / 180) * speed;
        double deltaY = sin((-main->angle + 90) * M_PI / 180) * speed;

        main->coord.setX(main->coord.x() + deltaX);
        main->coord.setY(main->coord.y() + deltaY);
    }
    if (main->leftPressed)
        main->angle -= 1;

    if (main->rightPressed)
        main->angle += 1;

    if (main->downPressed) {
        double deltaX = -cos((-main->angle + 90) * M_PI / 180) * speed;
        double deltaY = -sin((-main->angle + 90) * M_PI / 180) * speed;

        main->coord.setX(main->coord.x() + deltaX);
        main->coord.setY(main->coord.y() + deltaY);
    }
}

