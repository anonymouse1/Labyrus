#include "player.h"

Player::Player(int lat, QObject *parent) : QThread(parent) {
    coord = new QPointF(0.4, 0.4);
    latency = lat;
}

void Player::run() {
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);

    refresh = new QTimer;
    refresh->setInterval(10000);
    QObject::connect(refresh, SIGNAL(timeout()), this, SLOT(refreshTime()), Qt::DirectConnection);
    QObject::connect(server, SIGNAL(sendFields()), this, SLOT(refreshTime()), Qt::DirectConnection);
    QObject::connect(server, SIGNAL(forAllClientsPrint(QString)), this, SLOT(printString(QString)));

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
    server->names.remove(name);
    this->terminate();
}

void Player::sendHeroTime() {
    server->sendHeroesToPlayer(this);
}

void Player::printString(QString s) {
    sendingInformation.lock();
    socket->write((s + "\n").toAscii());
    sendingInformation.unlock();
}
