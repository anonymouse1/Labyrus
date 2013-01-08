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
    failConnection = new QTimer;
    repaintTimer = new QTimer;

    this->setWindowTitle(ip.toString() + ":" + QString::number(port) +" by " + login);

    qDebug() << ip.toString();
    mainSocket = new QTcpSocket(this);

    QObject::connect(mainSocket, SIGNAL(connected()), this,  SLOT(connectionEstablished()));
    QObject::connect(failConnection, SIGNAL(timeout()), this, SLOT(connectionFailed()));
    QObject::connect(repaintTimer, SIGNAL(timeout()), widget, SLOT(repaint()));

    nap = 0;
    scanN = true;
    otherHeroes = 0;
    fullRefresh = true;
    numberArsenals = 0;


    mainSocket->connectToHost(ip, port, QTcpSocket::ReadWrite);
    failConnection->setInterval(5000);
    repaintTimer->setInterval(16);

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
    coord.setX(0);
    coord.setY(0);
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
    command->go("Hello maze", true);
    command->go(login, true, false);

    mainSocket->waitForReadyRead(300);
    QString s = mainSocket->readLine();
    if (s != "success\n") {
        QMessageBox::critical(this, "Cannot connect to server", s);
        connectionFailed();
        this->close();
    } else {
        command->wasPrinted("Success\n");
        failConnection->stop();
        failConnection->deleteLater();
        emit successConnection();
    }

    mainSocket->waitForReadyRead(300);
    myDescriptor = scanInt();
    command->wasPrinted("as" + QString::number(myDescriptor));
    QObject::connect(mainSocket, SIGNAL(readyRead()), this, SLOT(readInformation()));

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

void MainWindow::keyPressEvent(QKeyEvent *event) {
    int key = event->key();
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

double MainWindow::scanInt() {
    if (!mainSocket->canReadLine())
        if (!mainSocket->waitForReadyRead(lacency))
            qDebug() << "slow net bugs enabled";
    QString s = mainSocket->readLine();
    if (s == "") {
        qDebug() << "got empty string";
        return 0;
    }
    if (s[s.length() - 1] == '\n')
        s.remove(s.length() - 1, 1);

    bool res;
    s.toDouble(&res);
    if (!res)
        qDebug() << "error scanning int from:" + s;
    return s.toDouble();
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
            processInformation();
            gameStart();
        } else if (s == "field\n") {
            readField();
        } else if (s == "hero\n") {
            readHeroes();
        } else {
            qDebug() << "unknown information" << s;
            qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
        }
    }
}

void MainWindow::processInformation() {
    mainSocket->waitForReadyRead(lacency);
    if (mainSocket->canReadLine())
        readInformation();
}

void MainWindow::readHeroes() {
    otherHeroes = scanInt();
    for (int i = 0; i < otherHeroes; i++) {
        int tmp = scanInt();
        QPointF c;
        c.setX(scanInt());
        c.setY(scanInt());
        if (tmp == myDescriptor) {
            if (fullRefresh) {
                coord = c;
                fullRefresh = false;
                heroes[i].setX(-1);
                heroes[i].setY(-1);
            }
        } else {
            heroes[i] = c;
            descriptors[i] = tmp;
        }
        mainSocket->waitForReadyRead(lacency);
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

bool MainWindow::equal(QPointF a, QPointF b) {
    return (fabs(a.x() - b.x()) < 1) && (fabs(a.y() - b.y()) < 1);
}
