#include "player.h"

Player::Player(int lat, QObject *parent) : QThread(parent) {
    coord.x = 0.5;
    coord.y = 0.5;
    coord.h = 0.5;
    latency = lat;
    QObject::connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void Player::run() {
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);

    refresh = new QTimer;
    refresh->setInterval(300000);
    refresh->start();

    checkValid = new QTimer;
    checkValid->setInterval(5000);
    checkValid->start();

    QObject::connect(refresh, SIGNAL(timeout()), this, SLOT(refreshTime()), Qt::DirectConnection);
    QObject::connect(server, SIGNAL(sendFields()), this, SLOT(refreshTime()), Qt::DirectConnection);
    QObject::connect(server, SIGNAL(forAllClientsPrint(QString)), this, SLOT(printString(QString)), Qt::DirectConnection);
    QObject::connect(checkValid, SIGNAL(timeout()), this, SLOT(disconnect()));

    if (server->radiation)
        QObject::connect(server->radiationTimer, SIGNAL(timeout()), this, SLOT(radiation()));

    sendHeroesTime = new QTimer;
    sendHeroesTime->setInterval(latency);
    server->processConnection(this);

    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(disconnect()), Qt::DirectConnection);
    QObject::connect(sendHeroesTime, SIGNAL(timeout()), this, SLOT(sendHeroTime()), Qt::DirectConnection);

    sendHeroesTime->start();
    exec();
}

void Player::refreshTime() {
    server->sendFieldToPlayer(this);
    server->sendHeroesToPlayer(this);
}

void Player::readyRead() {
    server->runCommand(socket->readLine(), this);
}

void Player::disconnect() {
    server->alreadyPlayers--;
    server->r.remove(socketDescriptor);
    qDebug() << this->name << "disconnected";
    emit say("S\n" + name + " disconnected\n");
    server->names.remove(name);
    exit(0);
}

void Player::sendHeroTime() {
    server->sendHeroesToPlayer(this);
}

void Player::printString(QString s) {
    sendingInformation.lock();
    socket->write((s + "\n").toLocal8Bit());
    sendingInformation.unlock();
}

void Player::radiation() {
    sendingInformation.lock();
    socket->write(QString("rad\n").toLocal8Bit());
    sendingInformation.unlock();
}

void Player::setValid() {
    checkValid->start();
}

void Player::flush() {
    socket->flush();
}
