#include "player.h"

Player::Player(QObject *parent) : QThread(parent) {
}

void Player::run() {
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);

    refresh = new QTimer;
    refresh->setInterval(10000);
    connect(refresh, SIGNAL(timeout()), this, SLOT(refreshTime()));

    server->processConnection(this);
    qDebug() << socket->thread();


    exec();
}

void Player::refreshTime() {
    server->sendFieldToPlayer(this);
    server->sendHeroesToPlayer(this);
}
