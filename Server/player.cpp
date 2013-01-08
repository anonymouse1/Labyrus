#include "player.h"

Player::Player(QObject *parent) : QThread(parent) {
    coord = new QPointF(0.4, 0.4);
}

void Player::run() {
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);

    refresh = new QTimer;
    refresh->setInterval(10000);
    QObject::connect(refresh, SIGNAL(timeout()), this, SLOT(refreshTime()));

    sendHeroesTime = new QTimer;
    sendHeroesTime->setInterval(lacency);
    sendHeroesTime->start();
    QObject::connect(sendHeroesTime, SIGNAL(timeout()), this, SLOT(sendHeroTime()));
//    sendHeroesTime->singleShot(3000, this, SLOT(sendHeroTime()));

    server->processConnection(this);

    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(disconnect()));
//    QObject::connect(server->sendHeroesTime, SIGNAL(timeout()), this, SLOT(sendHeroTime()));

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
    server->sendHeroes();
    qDebug() << this->name << "disconnected";
    this->terminate();
}

void Player::sendHeroTime() {
    server->sendHeroesToPlayer(this);
}
