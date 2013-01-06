#include "player.h"

Player::Player(QObject *parent) : QThread(parent) {
    coord = new QPointF(0.4, 0.4);
}

void Player::run() {
    this->moveToThread(this);
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);

    refresh = new QTimer;
    refresh->setInterval(10000);
    QObject::connect(refresh, SIGNAL(timeout()), this, SLOT(refreshTime()));


    server->processConnection(this);

    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(disconnect()));
    qDebug() << socket->thread();
    qDebug() << this;

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
    this->terminate();
    server->alreadyPlayers--;
    server->r.remove(socketDescriptor);
    server->sendHeroes();

    qDebug() << this->name << "disconnected";
}
