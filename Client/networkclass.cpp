#include "networkclass.h"

NetworkClass::NetworkClass(QHostAddress ip, quint16 port, QString myName, QThread *parent) :
    QThread(parent)
{
    login = myName;
    targetIp = ip;
    targetPort = port;
    yAngle = -80;
    angle = 45;
    cheats = false;
    fullRefresh = true;
    messages = new MessagesStack;
    escapeMode = false;
    allowWin = true;
}

void NetworkClass::run() {
    pingTimer = new QTimer();
    pingTimer->setInterval(1000);
    QObject::connect(pingTimer, SIGNAL(timeout()), this, SLOT(ping()));
    serverRefresh = new QTimer();
    QObject::connect(serverRefresh, SIGNAL(timeout()), this, SLOT(refreshCoords()));

    mainSocket = new QTcpSocket();

    mainSocket->moveToThread(this);
    pingTimer->moveToThread(this);
    serverRefresh->moveToThread(this);

    connect(mainSocket, SIGNAL(connected()), this, SLOT(connectionEstablished()), Qt::DirectConnection);

    mainSocket->connectToHost(targetIp, targetPort, QTcpSocket::ReadWrite);

    failConnectionTime = new QTimer;
    failConnectionTime->setInterval(5000);
    failConnectionTime->start();
    connect(failConnectionTime, SIGNAL(timeout()), this, SLOT(failConnection()), Qt::DirectConnection);

    exec();
}

void NetworkClass::readField() {
    n = scanInt();
    h = scanInt();
    m = scanInt();
    for (int i = 0; i < m; i++)
        for (int j = 0; j < 4; j++)
            walls[i][j] = scanInt();

    qDebug() << "fieldUpdate" << QTime::currentTime();
}

double NetworkClass::scanInt() {
    if (!mainSocket->isValid()) {
        qDebug() << "disconnected by socket";
        return 0;
    }
    if (!mainSocket->canReadLine())
        if (!mainSocket->waitForReadyRead(latency))
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

void NetworkClass::readInformation() {
    if (!processingInformation.tryLock())
        return;

    while (mainSocket->canReadLine()) {
        QString s = QString::fromLocal8Bit(mainSocket->readLine());
        if ((s == "") || (s == "\n"))
            continue;
        if (s == "gameStart\n") {
            qDebug() << "gameStart detected";
            emit gameStart();
            serverRefresh->start();
        } else if (s == "field\n") {
            readField();
        } else if (s == "hero\n") {
            readHeroes();
        } else if (s == "S\n") {
            messages->addMessage(QString::fromLocal8Bit(mainSocket->readLine()));
        } else if (s == "cheats\n") {
            cheats = true;
        } else if (s == "rad\n") {
            radiation = true;
        } else if (s == "p\n") {
            messages->addMessage(tr("Ping time: ") + QString::number(pingTime->elapsed()) + tr("ms"));
        } else if (s == "w\n") {
            winners.clear();
            int n;
            n = scanInt();
            for (int i = 0; i < n; i++) {
                winners.push_back(mainSocket->readLine());
                winners[i] = winners[i].left(winners[i].size() - 1);
            }
            qDebug() << winners;
        } else if (s == "nowin\n") {
            allowWin = false;
        } else if (s == "restart\n") {
            qDebug() << "restarting...";
            coord.h = 0.5;
            coord.x = 0.5;
            coord.y = 0.5;
        } else {
            qDebug() << "unknown information" << s;
            qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
        }
    }
    processingInformation.unlock();
}

void NetworkClass::readHeroes() {
    otherHeroes = scanInt();
    for (int i = 0; i < otherHeroes; i++) {
        int tmp = scanInt();
        fpoint c;
        c.x = scanInt();
        c.y = scanInt();
        c.h = scanInt();
        if (tmp == myDescriptor) {
            if (fullRefresh) {
                coord = c;
                fullRefresh = false;
            }
            heroes[i].x = -1000000;
        } else {
            heroes[i] = c;
            descriptors[i] = tmp;
        }
        if (!mainSocket->canReadLine())
            mainSocket->waitForReadyRead(latency);

        heroNames[i] = QString::fromLocal8Bit(mainSocket->readLine());
        heroNames[i] = heroNames[i].left(heroNames[i].length() - 1);
    }
}

void NetworkClass::go(QString s, bool flush, bool addEndLine) {
    processingInformation.lock();
    if (addEndLine)
        s += "\n";
    mainSocket->write(s.toLocal8Bit());
    if (flush)
        mainSocket->flush(); // it was a many strange bugs, was not it?
    processingInformation.unlock();
}

void NetworkClass::connectionEstablished() {
    QObject::connect(mainSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    qDebug() << "connection established";
    go("Hello maze\n" + login, true);
    qDebug() << mainSocket->state();
    if (!mainSocket->canReadLine())
        mainSocket->waitForReadyRead(1000);

    QString s = mainSocket->readLine();
    if (s != "success\n") {
        qDebug() << "unsuccess" << s;
        emit connectionFailed();
        return;
    } else {
        failConnectionTime->stop();
        failConnectionTime->deleteLater();
        emit successConnection();
    }

    myDescriptor = scanInt();
    latency = scanInt();
    qDebug() << "latency" << latency;
    connect(mainSocket, SIGNAL(readyRead()), this, SLOT(readInformation()), Qt::DirectConnection);
    pingTimer->start();
    serverRefresh->setInterval(latency);

    if (mainSocket->canReadLine())
        readInformation();
}

void NetworkClass::failConnection() {
    emit connectionFailed();
}

void NetworkClass::checkAngles() {
    if (yAngle > 0)
        yAngle = 0;

    if (yAngle < -180)
        yAngle = -180;
}

void NetworkClass::ping() {
    go("v");
}

int NetworkClass::getFloor() {
    if (coord.h < 0)
        return -int(-coord.h) - 1;
    else
        return coord.h;
}

void NetworkClass::refreshCoords() {
    go("n\n" + QString::number(coord.x) + "\n" + QString::number(coord.y) + "\n" + QString::number(coord.h) + "\n");
}

void NetworkClass::runCommand(QString s) {
    go(s);
}

bool NetworkClass::isAutonomous() {
    return mainSocket->state() != QTcpSocket::ConnectedState;
}

void NetworkClass::disconnected() {
    messages->addMessage(tr("You are disconnected from server"));
    messages->addMessage(tr("Autonomous mode"));
}
