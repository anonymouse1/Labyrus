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
    serverRefresh = new QTimer;

    QObject::connect(nextTimeTimer, SIGNAL(timeout()), this, SLOT(nextTime()));
    QObject::connect(serverRefresh, SIGNAL(timeout()), this, SLOT(refreshCoord()));

    nextTimeTimer->setInterval(10);
    nextTimeTimer->start();

    serverRefresh->setInterval(lacency);
    serverRefresh->start();

    exec();
}

void DrawThread::nextTime() {
    if (main->upPressed) {
        double deltaX = cos((-main->angle + 90) * M_PI / 180) * speed;
        double deltaY = sin((-main->angle + 90) * M_PI / 180) * speed;

        main->check(deltaX, deltaY);

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

        main->check(deltaX, deltaY);

        main->coord.setX(main->coord.x() + deltaX);
        main->coord.setY(main->coord.y() + deltaY);
    }

    if (main->leftStrife) {
        double deltaX = -cos(-main->angle * M_PI / 180) * speed;
        double deltaY = -sin(-main->angle * M_PI / 180) * speed;

        main->check(deltaX, deltaY);

        main->coord.setX(main->coord.x() + deltaX);
        main->coord.setY(main->coord.y() + deltaY);
    }

    if (main->rightStrife) {
        double deltaX = -cos((-main->angle + 180) * M_PI / 180) * speed;
        double deltaY = -sin((-main->angle + 180) * M_PI / 180) * speed;

        main->check(deltaX, deltaY);

        main->coord.setX(main->coord.x() + deltaX);
        main->coord.setY(main->coord.y() + deltaY);
    }

    for (int i = 0; i < main->numberArsenals; i++)
        if (main->equal(main->coord, main->arsenal[i])) {
            main->patrons += 3;
            main->wall += 1;
            main->destroy += 1;
            main->command->go(QString("a") + "\n" + QString::number(i), false);
            main->arsenal[i] = QPoint(-100, -100);
        }

    if (main->equal(main->coord, main->hospital)) {
        main->alive = true;
        main->sending.lock();
        main->command->go("l", false);
        main->sending.unlock();
    }
}

void DrawThread::refreshCoord() {
    main->sending.lock();
    main->command->go(QString("n\n") + QString::number(main->coord.x()) + "\n" + QString::number(main->coord.y()) + "\n", false);
    main->sending.unlock();
}
