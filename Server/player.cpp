#include "player.h"

Player::Player(QObject *parent) : QThread(parent) {
}

void Player::run() {
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    server->processConnection(this);
    qDebug() << socket->thread();

    exec();
}
