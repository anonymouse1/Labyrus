#include "mainwindow.h"

#define GL true
#define cheat false

const double animXY = 0.01;
const double animZRotone = 1;

MainWindow::MainWindow(QApplication *a, QHostAddress ip, quint16 port, QByteArray l,  QString skin, QWidget *parent) :
    QWidget(parent)
{
    n = 0;
    m = 0;

    app = a;

    widget = new DrawGl(app, skin);

    thread = new DrawThread(widget, this, this);
    thread->setPriority(QThread::LowestPriority);

    login = l;
    animateTimer = new QTimer;
    failConnection = new QTimer;
    repaintTimer = new QTimer;

    this->setWindowTitle(ip.toString() + ":" + QString::number(port) +" by " + login);

    qDebug() << ip.toString();
    mainSocket = new QTcpSocket(this);

    QObject::connect(mainSocket, SIGNAL(connected()), this, SLOT(connectionEstablished()));
    QObject::connect(animateTimer, SIGNAL(timeout()), this, SLOT(animate()));
    QObject::connect(failConnection, SIGNAL(timeout()), this, SLOT(connectionFailed()));
    QObject::connect(repaintTimer, SIGNAL(timeout()), widget, SLOT(repaint()));

    nap = 0;
    scanN = true;
    otherHeroes = 0;
    fullRefresh = true;
    numberArsenals = 0;


    mainSocket->connectToHost(ip, port, QTcpSocket::ReadWrite);
    animateTimer->setInterval(4);
    failConnection->setInterval(5000);
    repaintTimer->setInterval(16);

    animateTimer->start();
    failConnection->start();
    repaintTimer->start();

    widget->a = this;
    widget->setFocus();
//    widget->showFullScreen();

    upPressed = false;
    downPressed = false;
    leftPressed = false;
    rightPressed = false;
    leftStrife = false;
    rightStrife = false;
    angle = 45;
    coord.setX(0.5);
    coord.setY(0.5);
}

MainWindow::~MainWindow()
{
    qDebug() << "destroying";
    mainSocket->disconnectFromHost();
//    delete ui;
}

void MainWindow::close() {
    qDebug() << "close called";
    mainSocket->disconnectFromHost();
    deleteLater();
}

void MainWindow::connectionEstablished() {
    command->go("Hello maze");
    command->go(login, false);

    mainSocket->waitForReadyRead(300);
    QString s = mainSocket->readLine();
    if (s != "success\n") {
        QMessageBox::critical(this, "Cannot connect to server", s);
        connectionFailed();
        this->close();
    } else {
        command->wasPrinted("Success\n");
        failConnection->stop();
        delete failConnection;
        emit successConnection();
    }

    mainSocket->waitForReadyRead(300);
    myDescriptor = scanInt();
    command->wasPrinted("as" + QString::number(myDescriptor));
    QObject::connect(mainSocket, SIGNAL(readyRead()), this, SLOT(readInformation()));
    QObject::connect(command, SIGNAL(startBot()), this, SLOT(startBot()));

    if (mainSocket->canReadLine())
        readInformation();
}

void MainWindow::readField() {
    n = scanInt();
    m = scanInt();
    for (int i = 0; i < m; i++)
        for (int j = 0; j < 3; j++)
            walls[i][j] = scanInt();

    hospital.setX(scanInt());
    hospital.setY(scanInt());

    numberArsenals = scanInt();
    for (int i = 0; i < numberArsenals; i++) {
        arsenal[i].setX(scanInt());
        arsenal[i].setY(scanInt());
    }
}

int MainWindow::getRealX(double x) {
    return (this->width() / n - 1) * x + 1;
}

int MainWindow::getRealY(double y) {
    return (this->height() / n - 1) * y + 23;
}

/*int MainWindow::lefter() {
    return (nap + 3) % 4;
}

int MainWindow::righter() {
    return (nap + 1) % 4;
}

int MainWindow::backward() {
    return (nap + 2) % 4;
}


void MainWindow::fgdown() {
    if (noWall(backward()))
        command->go(QString::number(backward()));
}

void MainWindow::fgup() {
    if (noWall(nap))
        command->go(QString::number(nap));
}

void MainWindow::fgleft() {
    if (noWall(lefter()))
        command->go(QString::number(lefter()));
}

void MainWindow::fgright() {
    if (noWall(righter()))
        command->go(QString::number(righter()));
}

bool MainWindow::noWall(int d) {
    if (d == 0) {
        if (!isWallUp(coord)) {
            coord.setY(coord.y() - 1);
            widget->animY += 1;
            return true;
        }
    } else if (d == 1) {
        if (!isWallRight(coord)) {
            coord.setX(coord.x() + 1);
            widget->animX -= 1;
            return true;
        }
    } else if (d == 2) {
        if (!isWallDown(coord)) {
            coord.setY(coord.y() + 1);
            widget->animY -= 1;
            return true;
        }
    } else if (d == 3) {
        if (!isWallLeft(coord)) {
            coord.setX(coord.x() - 1);
            widget->animX += 1;
            return true;
        }
    }

    return false;
}*/

void MainWindow::keyPressEvent(QKeyEvent *event) {
    int key = event->key();
    qDebug() << key << "pressed";

    if (event->key() == Qt::Key_Z)
        if (!widget->isFullScreen())
            widget->showFullScreen();
        else
            widget->showNormal();

    if (event->key() == Qt::Key_I)
        widget->ytra += 0.01;
    else if (event->key() == Qt::Key_K)
        widget->ytra -= 0.01;
    else if (event->key() == Qt::Key_J)
        widget->xtra -= 0.01;
    else if (event->key() == Qt::Key_L)
        widget->xtra += 0.01;
    else if (event->key() == Qt::Key_U)
        widget->ztra += 0.01;
    else if (event->key() == Qt::Key_O)
        widget->ztra -= 0.01;

    if ((event->key() == Qt::Key_Up) || (event->key() == Qt::Key_W))
        upPressed = true;
    else if ((event->key() == Qt::Key_Down) || (event->key() == Qt::Key_S))
        downPressed = true;
    else if (event->key() == Qt::Key_Left)
        leftPressed = true;
    else if (event->key() == Qt::Key_Right)
        rightPressed = true;
    else if (event->key() == Qt::Key_A)
        leftStrife = true;
    else if (event->key() == Qt::Key_D)
        rightStrife = true;



    /*if ((widget->animX >= -1) && (widget->animX <= 1) && (widget->animY >= -1) &&
            (widget->animY <= 1) && (widget->animZRot <= 90) && (widget->animZRot >= -90)) {


    if ((key == Qt::Key_Up)) {
        fgdown();
    } else if (key == Qt::Key_Down) {
        fgup();
    } else if (key == Qt::Key_Left) {
        nap = righter();
        command->go("c\n" + QString::number(nap));
    } else if (key == Qt::Key_Right) {
        nap = lefter();
        command->go("c\n" + QString::number(nap));
    } else if (key == Qt::Key_S) {
        fgup();
    } else if (key == Qt::Key_A) {
        fgleft();
    } else if (key == Qt::Key_W) {
        fgdown();
    } else if (key == Qt::Key_D) {
        fgright();
    }

    if (event->key() == Qt::Key_Left) {
        widget->animZRot += -90;
    } else if (event->key() == Qt::Key_Right) {
        widget->animZRot += 90;
    }

    }*/

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

    for (int i = 0; i < numberArsenals; i++)
        if (coord == arsenal[i]) {
            patrons += 3;
            wall += 1;
            destroy += 1;
            command->go(QString("a") + "\n" + QString::number(i));
            arsenal[i] = QPoint(-100, -100);
        }

    if (coord == hospital) {
        alive = true;
        command->go("l");
    }

    seekNet();
    event->accept();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    qDebug() << "released key";
    if ((event->key() == Qt::Key_Up) || (event->key() == Qt::Key_W))
        upPressed = false;
    else if (event->key() == Qt::Key_Left)
        leftPressed = false;
    else if (event->key() == Qt::Key_Right)
        rightPressed = false;
    else if ((event->key() == Qt::Key_Down) || (event->key() == Qt::Key_S))
        downPressed = false;
    else if (event->key() == Qt::Key_A)
        leftStrife = false;
    else if (event->key() == Qt::Key_D)
        rightStrife = false;
}

int MainWindow::scanInt() {
    if (!mainSocket->canReadLine())
        if (mainSocket->waitForReadyRead(100))
            qDebug() << "slow net bugs enabled";
    QString s = mainSocket->readLine();
    if (s == "") {
        qDebug() << "got empty string";
        return 0;
    }
    if (s[s.length() - 1] == '\n')
        s.remove(s.length() - 1, 1);

    bool res;
    s.toInt(&res, 10);
    if (!res)
        qDebug() << "error scanning int from:" + s;
    return s.toInt();
}

/*bool MainWindow::isWallDown(QPoint c) {
    if (!cheat)
    for (int i = 0; i < m; i ++)
        if ((walls[i][0] == c.x()) && (walls[i][1] == c.y() + 1) && (walls[i][2] == 0))
            return true;

    return false;
}

bool MainWindow::isWallUp(QPoint c) {
    if (!cheat)
    for (int i = 0; i < m; i++)
        if ((walls[i][0] == c.x()) && (walls[i][1] == c.y()) && (walls[i][2] == 0))
            return true;

    return false;
}

bool MainWindow::isWallLeft(QPoint c) {
    if (!cheat)
    for (int i = 0; i < m; i++)
        if ((walls[i][0] == c.x()) && (walls[i][1] == c.y()) && (walls[i][2] == 1))
            return true;

    return false;
}

bool MainWindow::isWallRight(QPoint c) {
    if (!cheat)
    for (int i = 0; i < m; i++)
        if ((walls[i][0] == c.x() + 1) && (walls[i][1] == c.y()) && (walls[i][2] == 1))
            return true;

    return false;
}*/

void MainWindow::seekNet() {
    while (mainSocket->canReadLine())
        mainSocket->readLine();
}

void MainWindow::setFullRefresh() {
    fullRefresh = true;
}

void MainWindow::setUnScan() {
    scanN = false;
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

/*void MainWindow::startBot() {
    qDebug() << "bot started";
    widget->botActive = true;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            w[i][j] = false;

    superDfs();
    widget->botActive = false;
    qDebug() << "bot finished";
}*/

void MainWindow::strangeWait() {
    while ((widget->animZRot != 0) || (widget->animX != 0) || (widget->animY != 0))
        app->processEvents(QEventLoop::AllEvents, 10);
}

/*void MainWindow::syncNap(int a) {
    if (nap == (a + 3) % 4) {
        nap = righter();
        widget->animZRot += 90;
        strangeWait();
    } else {
        while (nap != a) {
            nap = lefter();
            widget->animZRot -= 90;
            strangeWait();
        }
    }
}*/

/*void MainWindow::standartMove(bool standart) {
    if (!standart)
        fgup();
    else
        fgdown();

    strangeWait();
    superDfs();
    if (!standart)
        fgup();
    else
        fgdown();

    strangeWait();
}*/

/*bool MainWindow::superDfs() {
    int a = nap;
    w[coord.x()][coord.y()] = true;
    if (!w[coord.x() - 1][coord.y()] && !isWallLeft(coord)) {
        syncNap(3);
        standartMove(false);
    }

    if (!w[coord.x()][coord.y() - 1] && !isWallUp(coord)) {
        syncNap(2);
        standartMove(true);
    }

    if (!w[coord.x()][coord.y() + 1] && !isWallDown(coord)) {
        syncNap(0);
        standartMove(true);
    }

    if (!w[coord.x() + 1][coord.y()] && !isWallRight(coord)) {
        syncNap(1);
        standartMove(false);
    }

    syncNap((a + 2) % 4);
    return 0;
}
*/
void MainWindow::gameStart() {
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

void MainWindow::animate() {
    //qDebug() << "animate Timer";

    if (widget->animZRot) {
        widget->zRot += widget->animZRot / abs(widget->animZRot) * animZRotone;
        widget->animZRot -= widget->animZRot / abs(widget->animZRot) * animZRotone;
    }

    if (fabs(widget->animZRot) < animZRotone) {
        widget->zRot += widget->animZRot;
        widget->animZRot = 0;
    }

    if (fabs(widget->animX) < animXY)
        widget->animX = 0;

    if (fabs(widget->animY) < animXY)
        widget->animY = 0;

    if (widget->animX > 0)
        widget->animX -= animXY;
    else if (widget->animX < 0)
        widget->animX += animXY;

    if (widget->animY > 0)
        widget->animY -= animXY;
    else if (widget->animY < 0)
        widget->animY += animXY;
}

double MainWindow::fabs(double a) {
    if (a < 0)
        return -a;
    else
        return a;
}

void MainWindow::connectionFailed() {
    emit fail();
}

void MainWindow::readInformation() {
    while (mainSocket->canReadLine()) {
        QString s = mainSocket->readLine();
        if (s == "gameStart\n") {
            qDebug() << "gameStart detected";
            thread->start();
            if (mainSocket->readLine() != "field\n")
                qDebug() << "very strange error";

            readField();
            mainSocket->waitForReadyRead(100);
            if (mainSocket->readLine() != "hero\n")
                qDebug() << "very strange error";

            readHeroes();
            gameStart();
            return;
        } else if (s == "field\n") {
            readField();
            return;
        } else if (s == "hero\n") {
            readHeroes();
        } else {
            qDebug() << "unknown information" << s;
            qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
        }
    }
}

void MainWindow::readHeroes() {
    otherHeroes = scanInt();
    for (int i = 0; i < otherHeroes; i++) {
        int tmp = scanInt();
        QPoint c;
        c.setX(scanInt());
        c.setY(scanInt());
        if (tmp == myDescriptor) {
            if (fullRefresh) {
                coord = c;
                qDebug() << coord;
            }

            fullRefresh = false;
            alive = scanInt();
            patrons = scanInt();
            wall = scanInt();
            destroy = scanInt();

            heroes[i].setX(-1);
            heroes[i].setY(-1);
        } else {
            heroes[i] = c;
            descriptors[i] = tmp;
            otherAlive[i] = scanInt();
            scanInt();
            scanInt();
            scanInt();
        }
        heroNames[i] = mainSocket->readLine();
        heroNames[i] = heroNames[i].left(heroNames[i].length() - 1);
    }
}

void MainWindow::checkForWall(double &dx, double &dy, double x1, double y1, double x2, double y2) {
    assert(x1 <= x2);
    assert(y1 <= y2);
    if (x1 == x2) {
        if ((coord.y() >= y1) && (coord.y() <= y2) && (fabs(coord.x() - x1) < 0.2) && ((x1 - coord.x() > 0) == (dx > 0)))
            dx = 0;
    } else
        if ((coord.x() >= x1) && (coord.x() <= x2) && (fabs(coord.y() - y1) < 0.2) && ((y1 - coord.y() > 0) == (dy > 0)))
            dy = 0;
}

void MainWindow::check(double &dx, double &dy) {
    double k = 1 / 10.0;
    for (int i = 0; i < m; i++)
        if (walls[i][2] == 0) {
            checkForWall(dx, dy, walls[i][0], walls[i][1], walls[i][0] + 1, walls[i][1]);
            checkForWall(dx, dy, walls[i][0], walls[i][1] - k, walls[i][0], walls[i][1] + k);
            checkForWall(dx, dy, walls[i][0] + 1, walls[i][1] - k, walls[i][0] + 1, walls[i][1] + k);
        } else {
            checkForWall(dx, dy, walls[i][0], walls[i][1], walls[i][0], walls[i][1] + 1);
            checkForWall(dx, dy, walls[i][0] - k, walls[i][1], walls[i][0] + k, walls[i][1]);
            checkForWall(dx, dy, walls[i][0] - k, walls[i][1] + 1, walls[i][0] + k, walls[i][1] + 1);
        }
}
