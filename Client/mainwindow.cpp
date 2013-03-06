#include "mainwindow.h"

const double animXY = 0.01;
const double animZRotone = 1;

MainWindow::MainWindow(QApplication *a, QHostAddress ip, quint16 port, QByteArray l,  QString skin, QObject *parent) :
    QObject(parent)
{
    app = a;
    repaintTimer = new QTimer;
    repaintTimer->setInterval(16);
    checkOrDie = new QTimer;
    checkOrDie->setInterval(5000);
    stopBot = false;
    widget = new DrawGl(app, skin);
    widget->legacy = this;
    checkOrDie->start();
    login = l;
    //    this->setWindowTitle(ip.toString() + ":" + QString::number(port) +" by " + login);

    qDebug() << ip.toString();
    input = new NetworkClass(ip, port, QString::fromLocal8Bit(login));

    QObject::connect(repaintTimer, SIGNAL(timeout()), widget, SLOT(repaint()));
    QObject::connect(input, SIGNAL(gameStart()), this, SLOT(gameStart()));
    QObject::connect(input, SIGNAL(connectionFailed()), this, SLOT(connectionFailed()));
    QObject::connect(input, SIGNAL(successConnection()), this, SLOT(connectedSuccess()));
    QObject::connect(widget, SIGNAL(destroyed()), this, SLOT(legalStop()));
    QObject::connect(checkOrDie, SIGNAL(timeout()), this, SLOT(checkForDie()));

    nap = 0;
    ctrlPressed = false;
    widget->a = input;
    repaintTimer->start();
    input->start();
    thread = new CalculationThread(widget, input);
}

MainWindow::~MainWindow() {
    qDebug() << "destroying";
}

void MainWindow::close() {
    qDebug() << "close called";
    deleteLater();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    int key = event->key();
    if (event->key() == Qt::Key_Z)
        if (!widget->isFullScreen())
            widget->showFullScreen();
        else
            widget->showNormal();

    if (input->cheats)
        if (event->key() == Qt::Key_Q)
            widget->ztra += 0.01;
        else if (event->key() == Qt::Key_E)
            widget->ztra -= 0.01;

    if (event->key() == Qt::Key_W)
        thread->upPressed = true;
    else if (event->key() == Qt::Key_S)
        thread->downPressed = true;
    else if (event->key() == Qt::Key_Left)
        thread->leftPressed = true;
    else if (event->key() == Qt::Key_Right)
        thread->rightPressed = true;
    else if (event->key() == Qt::Key_A)
        thread->leftStrife = true;
    else if (event->key() == Qt::Key_D)
        thread->rightStrife = true;
    else if (event->key() == Qt::Key_Up)
        thread->lookingUp = true;
    else if (event->key() == Qt::Key_Down)
        thread->lookingDown = true;
    else if (event->key() == Qt::Key_Shift)
        thread->shiftPressed = true;
    else if (event->key() == Qt::Key_Control) {
        ctrlPressed = true;
        backupPerspective = widget->perspective;
        widget->perspective = 10;
        widget->needRefreshCursor = false;
        widget->resize(widget->width(), widget->height() - 1);
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
    if (event->key() == Qt::Key_W)
        thread->upPressed = false;
    else if (event->key() == Qt::Key_Left)
        thread->leftPressed = false;
    else if (event->key() == Qt::Key_Right)
        thread->rightPressed = false;
    else if (event->key() == Qt::Key_S)
        thread->downPressed = false;
    else if (event->key() == Qt::Key_A)
        thread->leftStrife = false;
    else if (event->key() == Qt::Key_D)
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
    qDebug() << "bot started";
    widget->botActive = true;
    for (int i = 0; i < input->n; i++)
        for (int j = 0; j < input->n; j++)
            w[i][j] = false;

    integerCoord = getRealCoord();
    superDfs();
    widget->botActive = false;
    stopBot = false;
    qDebug() << "bot finished";
}

/*void MainWindow::strangeWait() {
    while ((widget->animZRot != 0) || (widget->animX != 0) || (widget->animY != 0))
        app->processEvents(QEventLoop::AllEvents, 10);
}*/

void MainWindow::syncNap(int a) {
    if (stopBot)
        return;

    while (input->angle > a + 2)
        input->angle -= 360;

    if (abs(input->angle - a) < 3)
        return;

    if (input->angle + 180 < a) {
        input->angle += 360;
        while (input->angle > a) {
            if (stopBot)
                break;
            thread->leftPressed = true;
            thread->rightPressed = false;
            app->processEvents(QEventLoop::AllEvents);
        }
    } else {
        while (input->angle < a) {
            if (stopBot)
                break;
            thread->leftPressed = false;
            thread->rightPressed = true;
            app->processEvents(QEventLoop::AllEvents);
        }
    }

    thread->leftPressed = false;
    thread->rightPressed = false;
}

void MainWindow::elementarMove(double x, double y) {
    if (stopBot)
        return;

    int time = thread->currentTime;
    while ((time + 90 > thread->currentTime) && (sqrt((x - input->coord.x()) * (x - input->coord.x()) + (y - input->coord.y()) * (y - input->coord.y())) > 0.1)) {
        if (stopBot)
            break;
        thread->upPressed = true;
        app->processEvents();
        sleep(1);
    }
    thread->upPressed = false;
}

void MainWindow::standartMove(double x1, double y1, double x2, double y2) {
    elementarMove(x2, y2);
    superDfs();
    syncNap(getAngle(x2, y2, x1, y1));
    elementarMove(x1, y1);
}

bool MainWindow::superDfs() {
    if (stopBot)
        return 1;

    integerCoord = getRealCoord();

    w[integerCoord.x()][integerCoord.y()] = true;

    int sp[4];
    for (int i = 0; i < 4; i++)
        sp[i] = i;
    for (int i = 1; i < 4; i++)
        swap(sp[i], sp[rand() % i]);

    for (int i = 0; i < 4; i++) {
        if (sp[i] == 0)
            if (!w[integerCoord.x() - 1][integerCoord.y()] && !isWallLeft(integerCoord)) {
                syncNap(getAngle(input->coord.x(), input->coord.y(), integerCoord.x() - 0.5, integerCoord.y() + 0.5));
                integerCoord.setX(integerCoord.x() - 1);
                standartMove(integerCoord.x() + 1.5, integerCoord.y() + 0.5, integerCoord.x() + 0.5, integerCoord.y() + 0.5);
                integerCoord.setX(integerCoord.x() + 1);
            }

        if (sp[i] == 1)
            if (!w[integerCoord.x()][integerCoord.y() + 1] && !isWallUp(integerCoord)) {
                syncNap(getAngle(input->coord.x(), input->coord.y(), integerCoord.x() + 0.5, integerCoord.y() + 1.5));
                integerCoord.setY(integerCoord.y() + 1);
                standartMove(integerCoord.x() + 0.5, integerCoord.y() - 0.5, integerCoord.x() + 0.5, integerCoord.y() + 0.5);
                integerCoord.setY(integerCoord.y() - 1);
            }

        if (sp[i] == 2)
            if (!w[integerCoord.x()][integerCoord.y() - 1] && !isWallDown(integerCoord)) {
                syncNap(getAngle(input->coord.x(), input->coord.y(), integerCoord.x() + 0.5, integerCoord.y() - 0.5));
                integerCoord.setY(integerCoord.y() - 1);
                standartMove(integerCoord.x() + 0.5, integerCoord.y() + 1.5, integerCoord.x() + 0.5, integerCoord.y() + 0.5);
                integerCoord.setY(integerCoord.y() + 1);
            }

        if (sp[i] == 3)
            if (!w[integerCoord.x() + 1][integerCoord.y()] && !isWallRight(integerCoord)) {
                syncNap(getAngle(input->coord.x(), input->coord.y(), integerCoord.x() + 1.5, integerCoord.y() + 0.5));
                integerCoord.setX(integerCoord.x() + 1);
                standartMove(integerCoord.x() - 0.5, integerCoord.y() + 0.5, integerCoord.x() + 0.5, integerCoord.y() + 0.5);
                integerCoord.setX(integerCoord.x() - 1);
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
    widget->ztra = -0.1 - (1 - a) * 0.4;
    widget->startAfter = startLine->currentTime();
}

void MainWindow::startingFinished() {
    qDebug() << "finishStarting";
    widget->startingGame = false;
    widget->startAfter = startLine->currentTime();
    widget->ztra = -0.1;
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

bool MainWindow::isWallLeft(QPoint c) {
    for (int i = 0; i < input->m; i++)
        if ((input->walls[i][0] == c.x()) && (input->walls[i][1] == c.y()) && (input->walls[i][2] == 1))
            return true;

    return false;
}

bool MainWindow::isWallRight(QPoint c) {
    for (int i = 0; i < input->m; i++)
        if ((input->walls[i][0] == c.x() + 1) && (input->walls[i][1] == c.y()) && (input->walls[i][2] == 1))
            return true;

    return false;
}
bool MainWindow::isWallUp(QPoint c) {
    for (int i = 0; i < input->m; i++)
        if ((input->walls[i][0] == c.x()) && (input->walls[i][1] == c.y() + 1) && (input->walls[i][2] == 0))
            return true;

    return false;
}
bool MainWindow::isWallDown(QPoint c) {
    for (int i = 0; i < input->m; i++)
        if ((input->walls[i][0] == c.x()) && (input->walls[i][1] == c.y()) && (input->walls[i][2] == 0))
            return true;

    return false;
}

void MainWindow::sleep(int ms) {
    QEventLoop *loop = new QEventLoop;
    QTimer::singleShot(ms, loop, SLOT(quit()));
    loop->exec();
}

QPoint MainWindow::getRealCoord() {
    qDebug() << "detecting coords..." << input->coord;
    QPoint result;
    for (int i = 0; i < input->n; i++)
        for (int j = 0; j < input->n; j++)
            if (fabs(result.x() - input->coord.x() + 0.5) + fabs(result.y() - input->coord.y() + 0.5) >
                    fabs(j - input->coord.x() + 0.5) + fabs(i - input->coord.y() + 0.5)) {
                result.setX(j);
                result.setY(i);
            }
    qDebug() << "coord" << result;
    return result;
}

int MainWindow::getAngle(double x, double y, double x1, double y1) {
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
