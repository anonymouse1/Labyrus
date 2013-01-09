#include "networkclass.h"

NetworkClass::NetworkClass(QHostAddress ip, quint16 port, QString myName, QThread *parent) :
    QThread(parent)
{
    login = myName;
    targetIp = ip;
    targetPort = port;
}

void NetworkClass::run() {
    mainSocket = new QTcpSocket(this);
    connect(mainSocket, SIGNAL(connected()), this, SLOT(connectionEstablished()), Qt::DirectConnection);

    mainSocket->connectToHost(targetIp, targetPort, QTcpSocket::ReadWrite);

    failConnectionTime = new QTimer;
    failConnectionTime->setInterval(5000);
    failConnectionTime->start();
    connect(failConnectionTime, SIGNAL(timeout()), this, SLOT(failConnection()), Qt::DirectConnection);

    angle = 45;
    fullRefresh = true;

    exec();
}

void NetworkClass::readField() {
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

    qDebug() << "fieldUpdate" << QTime::currentTime();
}

double NetworkClass::scanInt() {
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

void NetworkClass::readInformation() {
    processingInformation.lock();
    while (mainSocket->canReadLine()) {
        QString s = mainSocket->readLine();
        if ((s == "") || (s == "\n"))
            continue;
        if (s == "gameStart\n") {
            qDebug() << "gameStart detected";
            emit gameStart();
        } else if (s == "field\n") {
            readField();
        } else if (s == "hero\n") {
            readHeroes();
        } else {
            qDebug() << "unknown information" << s;
            qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
        }
    }
    processingInformation.unlock();
}

void NetworkClass::processInformation() {
    mainSocket->waitForReadyRead(lacency);
    if (mainSocket->canReadLine())
        readInformation();
}

void NetworkClass::readHeroes() {
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
        if (!mainSocket->canReadLine())
            mainSocket->waitForReadyRead(lacency);

        heroNames[i] = mainSocket->readLine();
        heroNames[i] = heroNames[i].left(heroNames[i].length() - 1);
    }
}

void NetworkClass::go(QString s, bool flush, bool addEndLine) {
    processingInformation.lock();
    if (addEndLine)
        s += "\n";
    mainSocket->write(s.toAscii());
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
        emit connectionFailed();
        return;
    } else {
        failConnectionTime->stop();
        failConnectionTime->deleteLater();
        emit successConnection();
    }

    myDescriptor = scanInt();
    connect(mainSocket, SIGNAL(readyRead()), this, SLOT(readInformation()), Qt::DirectConnection);

    if (mainSocket->canReadLine())
        readInformation();
}

void NetworkClass::failConnection() {
    emit connectionFailed();
}



