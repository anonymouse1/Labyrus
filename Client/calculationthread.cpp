#include "calculationthread.h"

CalculationThread::CalculationThread(DrawGl *wid, NetworkClass *m) : QThread()
{
    widget = wid;
    main = m;

    upPressed = false;
    downPressed = false;
    leftPressed = false;
    rightPressed = false;
    leftStrife = false;
    rightStrife = false;
}

void CalculationThread::run() {
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

void CalculationThread::nextTime() {
    if (upPressed) {
        double deltaX = cos((-main->angle + 90) * M_PI / 180) * speed;
        double deltaY = sin((-main->angle + 90) * M_PI / 180) * speed;

        check(deltaX, deltaY);

        main->coord.setX(main->coord.x() + deltaX);
        main->coord.setY(main->coord.y() + deltaY);
    }
    if (leftPressed)
        main->angle -= 1;

    if (rightPressed)
        main->angle += 1;

    if (downPressed) {
        double deltaX = -cos((-main->angle + 90) * M_PI / 180) * speed;
        double deltaY = -sin((-main->angle + 90) * M_PI / 180) * speed;

        check(deltaX, deltaY);

        main->coord.setX(main->coord.x() + deltaX);
        main->coord.setY(main->coord.y() + deltaY);
    }

    if (leftStrife) {
        double deltaX = -cos(-main->angle * M_PI / 180) * speed;
        double deltaY = -sin(-main->angle * M_PI / 180) * speed;

        check(deltaX, deltaY);

        main->coord.setX(main->coord.x() + deltaX);
        main->coord.setY(main->coord.y() + deltaY);
    }

    if (rightStrife) {
        double deltaX = -cos((-main->angle + 180) * M_PI / 180) * speed;
        double deltaY = -sin((-main->angle + 180) * M_PI / 180) * speed;

        check(deltaX, deltaY);

        main->coord.setX(main->coord.x() + deltaX);
        main->coord.setY(main->coord.y() + deltaY);
    }

    for (int i = 0; i < main->numberArsenals; i++)
        if (equal(main->coord, main->arsenal[i])) {
            main->patrons += 3;
            main->wall += 1;
            main->destroy += 1;
            main->go(QString("a") + "\n" + QString::number(i), false);
            main->arsenal[i] = QPoint(-100, -100);
        }

    if (equal(main->coord, main->hospital)) {
        main->alive = true;
        main->go("l");
    }
}

void CalculationThread::refreshCoord() {
    main->go(QString("n\n") + QString::number(main->coord.x()) + "\n" + QString::number(main->coord.y()) + "\n", false);
}

void CalculationThread::checkForWall(double &dx, double &dy, double x1, double y1, double x2, double y2) {
    assert(x1 <= x2);
    assert(y1 <= y2);
    if (x1 == x2) {
        if ((main->coord.y() >= y1) && (main->coord.y() <= y2) && (fabs(main->coord.x() - x1) < 0.2) && ((x1 - main->coord.x() > 0) == (dx > 0)))
            dx = 0;
    } else
        if ((main->coord.x() >= x1) && (main->coord.x() <= x2) && (fabs(main->coord.y() - y1) < 0.2) && ((y1 - main->coord.y() > 0) == (dy > 0)))
            dy = 0;
}

void CalculationThread::check(double &dx, double &dy) {
    double k = 1 / 10.0;
    for (int i = 0; i < main->m; i++)
        if (main->walls[i][2] == 0) {
            checkForWall(dx, dy, main->walls[i][0], main->walls[i][1], main->walls[i][0] + 1, main->walls[i][1]);
            checkForWall(dx, dy, main->walls[i][0], main->walls[i][1] - k, main->walls[i][0], main->walls[i][1] + k);
            checkForWall(dx, dy, main->walls[i][0] + 1, main->walls[i][1] - k, main->walls[i][0] + 1, main->walls[i][1] + k);
        } else {
            checkForWall(dx, dy, main->walls[i][0], main->walls[i][1], main->walls[i][0], main->walls[i][1] + 1);
            checkForWall(dx, dy, main->walls[i][0] - k, main->walls[i][1], main->walls[i][0] + k, main->walls[i][1]);
            checkForWall(dx, dy, main->walls[i][0] - k, main->walls[i][1] + 1, main->walls[i][0] + k, main->walls[i][1] + 1);
        }
}

bool CalculationThread::equal(QPointF a, QPointF b) {
    return (fabs(a.x() - b.x()) < 1) && (fabs(a.y() - b.y()) < 1);
}
