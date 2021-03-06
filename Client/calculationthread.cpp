#include "calculationthread.h"

CalculationThread::CalculationThread(DrawGl *wid, NetworkClass *m) : QThread() {
    widget = wid;
    main = m;

    upPressed = false;
    downPressed = false;
    leftPressed = false;
    rightPressed = false;
    leftStrife = false;
    rightStrife = false;
    lookingDown = false;
    lookingUp = false;
    shiftPressed = false;
    currentTime = 0;
    move = true;
}

void CalculationThread::run() {
    qDebug() << "run called";
    nextTimeTimer = new QTimer;

    QObject::connect(nextTimeTimer, SIGNAL(timeout()), this, SLOT(nextTime()));

    nextTimeTimer->setInterval(10);
    nextTimeTimer->start();

    fromStartOfGame.start();

    exec();
}

void CalculationThread::nextTime() {
    currentTime++;
    double deltaX = 0;
    double deltaY = 0;
    double deltaH = 0;
    if (upPressed && move) {
        deltaX += cos((-main->angle + 90) * M_PI / 180) * speed * (shiftPressed + 1) * max(cos((main->yAngle + 90) * M_PI / 180), main->h == 1);
        deltaY += sin((-main->angle + 90) * M_PI / 180) * speed * (shiftPressed + 1) * max(cos((main->yAngle + 90) * M_PI / 180), main->h == 1);
        deltaH += -sin((main->yAngle + 90) * M_PI / 180) * speed * (shiftPressed + 1);
    }
    if (leftPressed)
        main->angle -= 100 * speed;

    if (rightPressed)
        main->angle += 100 * speed;

    if (downPressed && move) {
        deltaX += -cos((-main->angle + 90) * M_PI / 180) * speed * max(cos((main->yAngle + 90) * M_PI / 180), main->h == 1);
        deltaY += -sin((-main->angle + 90) * M_PI / 180) * speed * max(cos((main->yAngle + 90) * M_PI / 180), main->h == 1);
        deltaH += sin((main->yAngle + 90) * M_PI / 180) * speed * (shiftPressed + 1);
    }

    if (leftStrife && move) {
        deltaX += -cos(-main->angle * M_PI / 180) * speed;
        deltaY += -sin(-main->angle * M_PI / 180) * speed;
        deltaH += 0;

    }

    if (rightStrife && move) {
        deltaX += -cos((-main->angle + 180) * M_PI / 180) * speed;
        deltaY += -sin((-main->angle + 180) * M_PI / 180) * speed;
        deltaH += 0;
    }

    if (lookingDown)
        main->yAngle += 1;

    if (lookingUp)
        main->yAngle -= 1;


    check(deltaX, deltaY, deltaH);

    main->coord.x += deltaX;
    main->coord.y += deltaY;
    main->coord.h += deltaH;

    main->checkAngles();
}

void CalculationThread::checkForWall(double &dx, double &dy, double x1, double y1, double x2, double y2) {
    if (x1 == x2) {
        if ((main->coord.y >= y1) && (main->coord.y <= y2) && (fabs(main->coord.x - x1) < radiusOfPlayer) && ((x1 - main->coord.x > 0) == (dx > 0)))
            dx = 0;
    } else
        if ((main->coord.x >= x1) && (main->coord.x <= x2) && (fabs(main->coord.y - y1) < radiusOfPlayer) && ((y1 - main->coord.y > 0) == (dy > 0)))
            dy = 0;
}

void CalculationThread::checkForDhWall(double &dh, double x, double y, double h, double x1, double y1) {
    if ((h + (dh < 0) - 1 == main->getFloor()) && (fabs(h - main->coord.h) < radiusOfPlayer) &&
            (main->coord.x >= x - eps) && (main->coord.x <= x1 + eps) &&
                (main->coord.y >= y - eps) && (main->coord.y <= y1 + eps))
                    dh = 0;
}

void CalculationThread::check(double &dx, double &dy, double &dh) {
    double k = 1 / 10.0;
    if (main->h == 1)
        dh = 0;
    for (int i = 0; i < main->m; i++)
        if (main->walls[i][3] == 0) {
            if (heightEqualToMe(main->walls[i][2]) || (main->h == 1)) {
                checkForWall(dx, dy, main->walls[i][0], main->walls[i][1], main->walls[i][0] + 1, main->walls[i][1]);
                checkForWall(dx, dy, main->walls[i][0], main->walls[i][1] - k, main->walls[i][0], main->walls[i][1] + k);
                checkForWall(dx, dy, main->walls[i][0] + 1, main->walls[i][1] - k, main->walls[i][0] + 1, main->walls[i][1] + k);
            }

            checkForDhWall(dh, main->walls[i][0], main->walls[i][1] - k * 1.5, main->walls[i][2], main->walls[i][0] + 1, main->walls[i][1] + k * 1.5);
            checkForDhWall(dh, main->walls[i][0], main->walls[i][1] - k * 1.5, main->walls[i][2] + 1, main->walls[i][0] + 1, main->walls[i][1] + k * 1.5);
        } else if (main->walls[i][3] == 1) {
            if ((heightEqualToMe(main->walls[i][2]) || (main->h == 1))) {
                checkForWall(dx, dy, main->walls[i][0], main->walls[i][1], main->walls[i][0], main->walls[i][1] + 1);
                checkForWall(dx, dy, main->walls[i][0] - k, main->walls[i][1], main->walls[i][0] + k, main->walls[i][1]);
                checkForWall(dx, dy, main->walls[i][0] - k, main->walls[i][1] + 1, main->walls[i][0] + k, main->walls[i][1] + 1);
            }

            checkForDhWall(dh, main->walls[i][0] - k * 1.5, main->walls[i][1], main->walls[i][2], main->walls[i][0] + k * 1.5, main->walls[i][1] + 1);
            checkForDhWall(dh, main->walls[i][0] - k * 1.5, main->walls[i][1], main->walls[i][2] + 1, main->walls[i][0] + k * 1.5, main->walls[i][1] + 1);
        } else if (main->walls[i][3] == 2) {
            checkForDhWall(dh, main->walls[i][0], main->walls[i][1], main->walls[i][2], main->walls[i][0] + 1, main->walls[i][1] + 1);
            if ((main->walls[i][2] - k < main->coord.h) && (main->coord.h < main->walls[i][2] + k)) {
                checkForWall(dx, dy, main->walls[i][0], main->walls[i][1], main->walls[i][0] + 1, main->walls[i][1]);
                checkForWall(dx, dy, main->walls[i][0], main->walls[i][1], main->walls[i][0], main->walls[i][1] + 1);
                checkForWall(dx, dy, main->walls[i][0] + 1, main->walls[i][1], main->walls[i][0] + 1, main->walls[i][1] + 1);
                checkForWall(dx, dy, main->walls[i][0], main->walls[i][1] + 1, main->walls[i][0] + 1, main->walls[i][1] + 1);
            }
        }
}

bool CalculationThread::equal(QPointF a, QPointF b) {
    return (fabs(a.x() - b.x()) < 1) && (fabs(a.y() - b.y()) < 1);
}

bool CalculationThread::heightEqualToMe(double height) {
    return (main->coord.h >= height) && (main->coord.h <= height + 1);
}

double CalculationThread::max(double a, double b) {
    if (a > b)
        return a;
    else
        return b;
}
