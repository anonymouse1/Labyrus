#include "mainwindow.h"

const double animXY = 0.01;
const double animZRotone = 1;

MainWindow::MainWindow(QApplication *a, QHostAddress ip, quint16 port, QByteArray l,  QString skin, double mouseSensitivity, QObject *parent) :
    QObject(parent)
{
    app = a;
    repaintTimer = new QTimer;
    repaintTimer->setInterval(16);
    checkOrDie = new QTimer;
    checkOrDie->setInterval(10000);
    stopBot = false;
    widget = new DrawGl(app, skin, mouseSensitivity);
    widget->legacy = this;
    widget->setMinimumHeight(480);
    widget->setMinimumWidth(640);
    checkOrDie->start();
    login = l;

    qDebug() << "connecting to " + ip.toString() + ":" + QString::number(port);
    input = new NetworkClass(ip, port, QString::fromLocal8Bit(login));

    QObject::connect(repaintTimer, SIGNAL(timeout()), widget, SLOT(repaint()));
    QObject::connect(input, SIGNAL(gameStart()), this, SLOT(gameStart()));
    QObject::connect(input, SIGNAL(connectionFailed()), this, SLOT(connectionFailed()));
    QObject::connect(input, SIGNAL(successConnection()), this, SLOT(connectedSuccess()));
    QObject::connect(widget, SIGNAL(destroyed()), this, SLOT(legalStop()));
    QObject::connect(checkOrDie, SIGNAL(timeout()), this, SLOT(checkForDie()));
    QObject::connect(widget, SIGNAL(runCommand(QString)), input, SLOT(runCommand(QString)));

    ctrlPressed = false;
    widget->a = input;
    repaintTimer->start();
    input->start();
    thread = new CalculationThread(widget, input);
}

MainWindow::~MainWindow() {
    if (widget->isValid())
        delete widget;
    qDebug() << "destroying";
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    int key = event->key();
    QString text = event->text().toUpper();
    if ((key == Qt::Key_Z) || (text == tr("Z"))) {
        if (!widget->isFullScreen())
            widget->showFullScreen();
        else
            widget->showNormal();
    }

    if (input->cheats) {
        if ((key == Qt::Key_Q) || (text == tr("Q")))
            input->coord.h += 0.1;
        else if ((key == Qt::Key_E) || (text == tr("E")))
            input->coord.h -= 0.1;
    }

    if ((key == Qt::Key_W) || (text == tr("W")))
        thread->upPressed = true;
    else if ((key == Qt::Key_S) || (text == tr("S")))
        thread->downPressed = true;
    else if (key == Qt::Key_Left)
        thread->leftPressed = true;
    else if (key == Qt::Key_Right)
        thread->rightPressed = true;
    else if ((key == Qt::Key_A) || (text == tr("A")))
        thread->leftStrife = true;
    else if ((key == Qt::Key_D) || (text == tr("D")))
        thread->rightStrife = true;
    else if (key == Qt::Key_Up)
        thread->lookingUp = true;
    else if (key == Qt::Key_Down)
        thread->lookingDown = true;
    else if (key == Qt::Key_Shift)
        thread->shiftPressed = true;
    else if (key == Qt::Key_Control) {
        if (ctrlPressed)
            return;
        ctrlPressed = true;
        backupPerspective = widget->perspective;
        widget->perspective = 10;
        widget->needRefreshCursor = false;
        #ifdef BEAUTIFULL_BUGS
            widget->resize(widget->width(), widget->height() - 1);
        #else
            widget->resize(widget->width(), widget->height() - 1);
            widget->resize(widget->width(), widget->height() + 1);
        #endif
    } else if (key == Qt::Key_Escape) {
        input->escapeMode = true;
    }



    /*if ((key == Qt::Key_Space) && (alive) && (patrons)) {
        taskKill();
    } else if ((key == Qt::Key_B) && (alive) && (wall)) {
        if (nap == 2)
            createWall(coord.x(), coord.y(), 0);
        else if (nap == 3)
            createWall(coord.x() + 1, coord.y(), 1);
        else if (nap == 0)
            createWall(coord.x(), coord.y() + 1, 0);
        else
            createWall(coord.x(), coord.y(), 1);
    } else if ((key == Qt::Key_E) && (alive) && (destroy)) {
        command->go("r");
        if (nap == 2)
            eraseWall(coord.x(), coord.y(), 0);
        else if (nap == 3)
            eraseWall(coord.x() + 1, coord.y(), 1);
        else if (nap == 0)
            eraseWall(coord.x(), coord.y() + 1, 0);
        else
            eraseWall(coord.x(), coord.y(), 1);
    }*/

    event->accept();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    int key = event->key();
    QString text = event->text().toUpper();
    if ((key == Qt::Key_W) || (text == tr("W")))
        thread->upPressed = false;
    else if (key == Qt::Key_Left)
        thread->leftPressed = false;
    else if (key == Qt::Key_Right)
        thread->rightPressed = false;
    else if ((key == Qt::Key_S) || (text == tr("S")))
        thread->downPressed = false;
    else if ((key == Qt::Key_A) || (text == tr("A")))
        thread->leftStrife = false;
    else if ((key == Qt::Key_D) || (text == tr("D")))
        thread->rightStrife = false;
    else if (event->key() == Qt::Key_Up)
        thread->lookingUp = false;
    else if (event->key() == Qt::Key_Down)
        thread->lookingDown = false;
    else if (event->key() == Qt::Key_Shift)
        thread->shiftPressed = false;
    else if (event->key() == Qt::Key_Control) {
        #ifndef BEAUTIFULL_BUGS
            if (!ctrlPressed)
                return;
        #endif
        ctrlPressed = false;
        widget->perspective = backupPerspective;
        widget->needRefreshCursor = false;
        widget->resize(widget->width(), widget->height() - 1);
        widget->resize(widget->width(), widget->height() + 1);
    }
}

/*void MainWindow::createWall(int x, int y, int flag) {
    command->go("b\n" + QString::number(x) + "\n" + QString::number(y) + "\n" + QString::number(flag));
}

void MainWindow::eraseWall(int x, int y, int flag) {
    qDebug() << "try to erase wall";
    for (int i = 0; i < m; i++)
        if ((walls[i][0] == x) && (walls[i][1] == y) && (walls[i][2] == flag))
            command->go("e\n" + QString::number(i));
}*/

void MainWindow::startBot() {
    input->messages->addMessage("BOT started");
    widget->botActive = true;
    for (int i = 0; i < input->n; i++)
        for (int j = 0; j < input->n; j++)
            for (int k = 0; k < input->h; k++)
                w[i][j][k] = false;

    integerCoord = getRealCoord();
    superDfs();
    widget->botActive = false;
    stopBot = false;
    input->messages->addMessage("BOT finished");
}

void MainWindow::syncNap(int a, int b) {
    if (stopBot)
        return;

    while (input->angle > a + 5)
        input->angle -= 360;

    if (abs(input->angle - a) > 3) {
        if (input->angle + 180 < a) {
            input->angle += 360;
            while (input->angle > a) {
                if (stopBot)
                    break;
                thread->leftPressed = true;
                app->processEvents(QEventLoop::AllEvents);
                sleep(5);
            }
        } else {
            while (input->angle < a) {
               if (stopBot)
                   break;
               thread->rightPressed = true;
               app->processEvents(QEventLoop::AllEvents);
               sleep(5);
            }
        }
    }

    thread->leftPressed = false;
    thread->rightPressed = false;


    if ((input->h != 1) && (abs(b + input->angle + 90) > 3)) {
        thread->lookingDown = false;
        bool turned = false;
        while (-input->yAngle - 90 < b) {
            if (stopBot)
                break;
            thread->lookingUp = true;
            app->processEvents(QEventLoop::AllEvents);
            sleep(5);
            turned = true;
        }

        thread->lookingUp = false;
        while (!turned && (-input->yAngle - 90 > b)) {
            if (stopBot)
                break;
            thread->lookingDown = true;
            app->processEvents(QEventLoop::AllEvents);
            sleep(5);
        }
    }

    thread->lookingDown = false;
    thread->lookingUp = false;
}

void MainWindow::elementarMove(fpoint to) {
    if (stopBot)
        return;

    int time = thread->currentTime;
    fpoint prev = input->coord;
    while (time + 85 > thread->currentTime) {
        prev = input->coord;
        if (stopBot)
            break;
        thread->upPressed = true;
        app->processEvents();
        sleep(5);
        if (sqrt(sqr(to.x - input->coord.x) + sqr(to.y - input->coord.y) + sqr(to.h - input->coord.h)) >
                sqrt(sqr(to.x - prev.x) + sqr(to.y - prev.y) + sqr(to.h - prev.h)))
            break;
    }
    thread->upPressed = false;
}

void MainWindow::standartMove(fpoint from, fpoint to) {
    syncNap(getAngle(input->coord.x, input->coord.y, to.x, to.y), getYAngle(sqrt(sqr(input->coord.x - to.x) + sqr(input->coord.y - to.y) + sqr(input->coord.h - to.h)), input->coord.h, to.h));
    elementarMove(to);
    superDfs();
    syncNap(getAngle(input->coord.x, input->coord.y, from.x, from.y), getYAngle(sqrt(sqr(from.x - input->coord.x) + sqr(from.y - input->coord.y) + sqr(from.h - input->coord.h)), input->coord.h, from.h));
    elementarMove(from);
}

bool MainWindow::superDfs() {
    if (stopBot)
        return 1;

    integerCoord = getRealCoord();

    w[integerCoord.x][integerCoord.y][integerCoord.h] = true;

    int sp[6];
    for (int i = 0; i < 6; i++)
        sp[i] = i;
    for (int i = 1; i < 6; i++)
        swap(sp[i], sp[rand() % i]);

    for (int i = 0; i < 6; i++) {
        integerCoord = getRealCoord();
        if (sp[i] == 0)
            if (!w[integerCoord.x - 1][integerCoord.y][integerCoord.h] && !isWallLeft(integerCoord)) {
                integerCoord.x -= 1;
                standartMove(genFPoint(integerCoord.x + 1.5, integerCoord.y + 0.5, integerCoord.h + 0.5),
                             genFPoint(integerCoord.x + 0.5, integerCoord.y + 0.5, integerCoord.h + 0.5));
                integerCoord.x += 1;
            }

        if (sp[i] == 1)
            if (!w[integerCoord.x][integerCoord.y + 1][integerCoord.h] && !isWallForward(integerCoord)) {
                integerCoord.y += 1;
                standartMove(genFPoint(integerCoord.x + 0.5, integerCoord.y - 0.5, integerCoord.h + 0.5),
                             genFPoint(integerCoord.x + 0.5, integerCoord.y + 0.5, integerCoord.h + 0.5));
                integerCoord.y -= 1;
            }

        if (sp[i] == 2)
            if (!w[integerCoord.x][integerCoord.y - 1][integerCoord.h] && !isWallBackward(integerCoord)) {
                integerCoord.y -= 1;
                standartMove(genFPoint(integerCoord.x + 0.5, integerCoord.y + 1.5, integerCoord.h + 0.5),
                             genFPoint(integerCoord.x + 0.5, integerCoord.y + 0.5, integerCoord.h + 0.5));
                integerCoord.y += 1;
            }

        if (sp[i] == 3)
            if (!w[integerCoord.x + 1][integerCoord.y][integerCoord.h] && !isWallRight(integerCoord)) {
                integerCoord.x += 1;
                standartMove(genFPoint(integerCoord.x - 0.5, integerCoord.y + 0.5, integerCoord.h + 0.5),
                             genFPoint(integerCoord.x + 0.5, integerCoord.y + 0.5, integerCoord.h + 0.5));
                integerCoord.x -= 1;
            }

        if (sp[i] == 4)
            if (!w[integerCoord.x][integerCoord.y][integerCoord.h + 1] && !isWallUp(integerCoord)) {
                integerCoord.h += 1;
                standartMove(genFPoint(integerCoord.x + 0.5, integerCoord.y + 0.5, integerCoord.h - 0.5),
                             genFPoint(integerCoord.x + 0.5, integerCoord.y + 0.5, integerCoord.h + 0.5));
                integerCoord.h -= 1;
            }
        if (sp[i] == 5)
            if (!w[integerCoord.x][integerCoord.y][integerCoord.h - 1] && !isWallDown(integerCoord)) {
                integerCoord.h -= 1;
                standartMove(genFPoint(integerCoord.x + 0.5, integerCoord.y + 0.5, integerCoord.h + 1.5),
                             genFPoint(integerCoord.x + 0.5, integerCoord.y + 0.5, integerCoord.h + 0.5));
                integerCoord.h += 1;
            }
    }

    return 0;
}


void MainWindow::gameStart() {
    thread->start();
    input->readInformation();

    qDebug() << "starting game";
    startLine = new QTimeLine;
    startLine->setDuration(3000);
    startLine->setUpdateInterval(10);

    QObject::connect(startLine, SIGNAL(valueChanged(qreal)), this, SLOT(startTimerChanged(qreal)));
    QObject::connect(startLine, SIGNAL(finished()), this, SLOT(startingFinished()));

    startLine->start();
    widget->startingGame = true;
}

void MainWindow::startTimerChanged(qreal a) {
    widget->ztra = -(1 - a) * 0.4;
    widget->startAfter = startLine->currentTime();
}

void MainWindow::startingFinished() {
    qDebug() << "finishStarting";
    widget->startingGame = false;
    widget->startAfter = startLine->currentTime();
    delete startLine;
}

/*void MainWindow::taskKill() {
    QPoint c = coord;
    patrons--;
    nap = backward();
    for (int i = 0; (i < 20) && (noWall(nap)); i++)
        for (int j = 0; j < otherHeroes; j++) {
            qDebug() << heroes[j] << coord;
            if (coord == heroes[j]) {
                command->go("p");
                command->go(QString::number(descriptors[j]));
                otherAlive[j] = false;
                qDebug() << "killing" << descriptors[j];
                break;
            }
        }

    nap = backward();
    coord = c;
    widget->animX = 0;
    widget->animY = 0;
}*/
double MainWindow::fabs(double a) {
    if (a < 0)
        return -a;
    else
        return a;
}

void MainWindow::connectionFailed() {
    qDebug() << "fail";
    emit fail();
}

void MainWindow::connectedSuccess() {
    qDebug() << "success";
    emit successConnection();
}

bool MainWindow::isWallLeft(gpoint c) {
    for (int i = 0; i < input->m; i++)
        if ((input->walls[i][0] == c.x) && (input->walls[i][1] == c.y) && (input->walls[i][3] == 1) && (input->walls[i][2] == c.h))
            return true;

    return false;
}

bool MainWindow::isWallRight(gpoint c) {
    for (int i = 0; i < input->m; i++)
        if ((input->walls[i][0] == c.x + 1) && (input->walls[i][1] == c.y) && (input->walls[i][3] == 1) && (input->walls[i][2] == c.h))
            return true;

    return false;
}
bool MainWindow::isWallForward(gpoint c) {
    for (int i = 0; i < input->m; i++)
        if ((input->walls[i][0] == c.x) && (input->walls[i][1] == c.y + 1) && (input->walls[i][3] == 0) && (input->walls[i][2] == c.h))
            return true;

    return false;
}
bool MainWindow::isWallBackward(gpoint c) {
    for (int i = 0; i < input->m; i++)
        if ((input->walls[i][0] == c.x) && (input->walls[i][1] == c.y) && (input->walls[i][3] == 0) && (input->walls[i][2] == c.h))
            return true;

    return false;
}

bool MainWindow::isWallDown(gpoint c) {
    for (int i = 0; i < input->m; i++)
        if ((input->walls[i][0] == c.x) && (input->walls[i][1] == c.y) && (input->walls[i][2] == c.h) && (input->walls[i][3] == 2))
            return true;

    return false;
}

bool MainWindow::isWallUp(gpoint c) {
    for (int i = 0; i < input->m; i++)
        if ((input->walls[i][0] == c.x) && (input->walls[i][1] == c.y) && (input->walls[i][2] == c.h + 1) && (input->walls[i][3] == 2))
            return true;

    return false;
}

void MainWindow::sleep(int ms) {
    QEventLoop *loop = new QEventLoop;
    QTimer::singleShot(ms, loop, SLOT(quit()));
    loop->exec();
}

gpoint MainWindow::getRealCoord() {
    qDebug() << "detecting coords..." << input->coord.x << input->coord.y << input->coord.h;
    gpoint result;
    for (int i = 0; i < input->n; i++)
        for (int j = 0; j < input->n; j++)
            if (fabs(result.x - input->coord.x + 0.5) + fabs(result.y - input->coord.y + 0.5) >
                    fabs(j - input->coord.x + 0.5) + fabs(i - input->coord.y + 0.5)) {
                result.x = j;
                result.y = i;
            }
    result.h =  input->getFloor();
    qDebug() << "result: " << result.x << result.y << result.h;
    return result;
}

int MainWindow::getAngle(double x, double y, double x1, double y1) {
    if (sqrt(sqr(x - x1) + sqr(y - y1)) < 0.2)
        return input->angle;
    x1 -= x;
    y1 -= y;
    double module = sqrt(x1 * x1 + y1 * y1);
    x1 /= module;
    y1 /= module;
    int result = acos(x1) / M_PI * 180;
    if (y1 < 0)
        result = 360 - result;

    return 90 - result;
}

int MainWindow::getYAngle(double rast, double h, double h1) {
    return asin((h1 - h) / (rast + eps)) / M_PI * 180;
}

void MainWindow::legalStop() {
    qDebug() << "stopping program";
    stopBot = true;
    if (thread->isRunning())
        thread->quit();
    if (input->isRunning())
        input->quit();
    app->quit();
}

void MainWindow::checkForDie() {
    if (!widget->isValid() || !widget->isVisible())
        legalStop();
}

fpoint MainWindow::genFPoint(double x, double y, double h) {
    fpoint result;
    result.x = x;
    result.y = y;
    result.h = h;
    return result;
}

double MainWindow::sqr(double a) {
    return a * a;
}
