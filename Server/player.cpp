#include "player.h"

Player::Player(int lat, QObject *parent) : QThread(parent) {
    coord.x = 0.5;
    coord.y = 0.5;
    coord.h = 0.5;
    latency = lat;
    QObject::connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void Player::run() {
    socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    refresh = new QTimer(this);
    refresh->setInterval(300000);
    refresh->start();

    checkValid = new QTimer(this);
    checkValid->setInterval(5000);
    checkValid->start();

    QObject::connect(refresh, SIGNAL(timeout()), this, SLOT(refreshTime()), Qt::DirectConnection);
    QObject::connect(server, SIGNAL(sendFields()), this, SLOT(refreshTime()));
    QObject::connect(server, SIGNAL(forAllClientsPrint(QString)), this, SLOT(printString(QString)));
    QObject::connect(checkValid, SIGNAL(timeout()), this, SLOT(lagger()));

    if (server->radiation)
        QObject::connect(server->radiationTimer, SIGNAL(timeout()), this, SLOT(radiation()));

    sendHeroesTime = new QTimer(this);
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
    if (server->alreadyPlayers == 0)
        server->gameStart = false;
    server->r.remove(socketDescriptor);
    server->alreadyPlayers = server->r.size();
    qDebug() << this->name << "disconnected";
    server->names.remove(name);
    QObject::disconnect(socket, SIGNAL(disconnected()), this, SLOT(disconnect()));
    QObject::disconnect(this, SIGNAL(say(QString)), server, SLOT(forAllClients(QString)));
    QObject::disconnect(server, SIGNAL(forAllClientsPrint(QString)), this, SLOT(printString(QString)));
    socket->deleteLater();
    emit say("S\n" + name + " disconnected\n");
    exit(0);
}

void Player::sendHeroTime() {
    server->sendHeroesToPlayer(this);
}

void Player::printString(QString s) {
    sendingInformation.lock();
    if (socket->state() != QTcpSocket::ConnectedState)
        disconnect();
    else
        socket->write((s + "\n").toLocal8Bit());
    sendingInformation.unlock();
}

void Player::radiation() {
    printString("rad");
}

void Player::setValid() {
    checkValid->start();
}

void Player::flush() {
    socket->flush();
}

void Player::lagger() {
    emit say("S\n" + name + " is lagger");
    disconnect();
}
