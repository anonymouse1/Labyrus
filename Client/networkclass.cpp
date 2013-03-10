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
}

void NetworkClass::run() {
    pingTimer = new QTimer(this);
    pingTimer->setInterval(1000);
    QObject::connect(pingTimer, SIGNAL(timeout()), this, SLOT(ping()));
    serverRefresh = new QTimer(this);
    QObject::connect(serverRefresh, SIGNAL(timeout()), this, SLOT(refreshCoords()));

    mainSocket = new QTcpSocket(this);

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
            messages->addMessage("Ping time: " + QString::number(pingTime->elapsed() + 1));
            qDebug() << pingTime->elapsed();
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
    qDebug() << "connection established" << this->currentThread();
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
    for (int i = 0; i <= h; i++)
        if (i + 1 >= coord.h)
            return i;

    return 0;
}

void NetworkClass::refreshCoords() {
    go("n\n" + QString::number(coord.x) + "\n" + QString::number(coord.y) + "\n" + QString::number(coord.h) + "\n");
}

void NetworkClass::runCommand(QString s) {
    go(s);
}
