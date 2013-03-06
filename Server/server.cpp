#include "server.h"

Server::Server(qint16 port, bool rad, bool cheat, bool sil, int size, int lat, int players, bool strong, QObject *parent) :
    QTcpServer(parent)
{
    if (!listen(QHostAddress::Any, port))
        die(errorString());

    alreadyPlayers = 0;
    silence = sil;

    latency = lat;
    numPlayers = players;
    strongNumPlayers = strong;
    cheats = cheat;
    radiation = rad;
    if (radiation) {
        radiationTimer = new QTimer;
        radiationTimer->setInterval(4000);
    }
    n = size;
    generateMap();
    QList<QHostAddress> l = QNetworkInterface::allAddresses();
    for (int i = 0; i < l.size(); i++)
        if (l[i].toIPv4Address() && l[i] != QHostAddress::LocalHost)
            qDebug() << QString("server is upped on ") + QString(l[i].toString()) + QChar(':') + QString::number(port);

    gameStart = false;
}

void Server::die(QString s) {
    qDebug() << s;
    exit(0);
}

void Server::incomingConnection(int handle) {
    qDebug() << "new connection detected " << handle;
    if (strongNumPlayers && (alreadyPlayers == numPlayers)) {
        qDebug() << "too many connections";
        return;
    }

    Player *player = new Player(latency);
    QObject::connect(player, SIGNAL(say(QString)), this, SLOT(forAllClients(QString)));
    player->socketDescriptor = handle;
    player->server = this;
    player->start();
}

void Server::processConnection(Player *player) {
//    QTcpSocket *socket = server->nextPendingConnection();
    qDebug() << QThread::currentThread();
    QTcpSocket *socket = player->socket;

    if (!socket->canReadLine())
        socket->waitForReadyRead(latency);

    QString s = socket->readLine();
    if (s != "Hello maze\n") {
        qDebug() << "this was unknown hacker" << s;
        socket->write("Are you a hacker?");
        socket->disconnectFromHost();
        return;
    }
    if (!socket->canReadLine())
        socket->waitForReadyRead(latency);
    player->name = QString::fromLocal8Bit(socket->readLine());
    player->name.remove(player->name.length() - 1, 1);
    if (names.find(player->name) != names.end()) {
        qDebug() << "used login";
        socket->write("login is already in use");
        socket->flush();
        socket->disconnectFromHost();
        player->terminate();
        return;
    }
    r[player->socketDescriptor] = player;
    names.insert(player->name);
    socket->write("success\n");
    socket->write((QString::number(socket->socketDescriptor()) + "\n").toAscii());
    socket->write((QString::number(latency) + "\n").toAscii());

    player->coord->setX(0.5);
    player->coord->setY(0.5);
    player->socket = socket;
    player->patrons = 3;
    player->walls = 1;
    player->alive = true;
    player->destiny = 0;
    player->destroy = 3;

    qDebug() << player->name << "connected";
    qDebug() << "as" << socket->socketDescriptor();
    alreadyPlayers++;

    if (alreadyPlayers >= numPlayers) {
        qDebug() << "starting game";
        gameStart = true;
        emit forAllClientsPrint("gameStart");
        if (cheats) {
            emit forAllClientsPrint("cheats");
            emit forAllClientsPrint("S\nCheats allowed");
        }

        if (radiation) {
            emit forAllClientsPrint("S\nThere is some radiation near");
            radiationTimer->start();
        }
    }

    sendFieldToPlayer(player);
    emit forAllClientsPrint("S\n" + player->name + " connected");
}

void Server::runCommand(QString command, Player *player) {
    if (command[0] == 'n') {
        QString s;
        s = player->socket->readLine();
        player->coord->setX(s.left(s.length() - 1).toDouble());
        s = player->socket->readLine();
        player->coord->setY(s.left(s.length() - 1).toDouble());
    } else if (command[0] == 'I') {
        forAllClientsPrint("S\n" + player->name + ": " + QString::fromLocal8Bit(player->socket->readLine()));
    } else if (command[0] == 'v') {
        player->setValid();
    } else if (command[0] == 'p') {
        player->socket->write("p\n");
    }

    if (player->socket->canReadLine())
        runCommand(player->socket->readLine(), player);
//    qDebug() << "success" << *player->coord;
}

void Server::sendFieldToPlayer(Player *player, QByteArray *data) {
    qDebug() << "sending field to player" << player->name << QTime::currentTime();
    player->sendingInformation.lock();
    if (data == NULL)
        data = generateFieldMessage();

    player->socket->write(*data);
    player->sendingInformation.unlock();
}

void Server::sendHeroesToPlayer(Player *player, QByteArray *data) {
    player->sendingInformation.lock();
    if (data == NULL)
        data = generateHeroMessage();

    player->socket->write(*data);
    player->sendingInformation.unlock();
}

bool Server::isWallDown(QPoint c) {
    for (int i = 0; i < m; i ++)
        if ((walls[i][0] == c.x()) && (walls[i][1] == c.y() + 1) && (walls[i][2] == 0))
            return true;

    return false;
}

bool Server::isWallUp(QPoint c) {
    for (int i = 0; i < m; i++)
        if ((walls[i][0] == c.x()) && (walls[i][1] == c.y()) && (walls[i][2] == 0))
            return true;

    return false;
}

bool Server::isWallLeft(QPoint c) {
    for (int i = 0; i < m; i++)
        if ((walls[i][0] == c.x()) && (walls[i][1] == c.y()) && (walls[i][2] == 1))
            return true;

    return false;
}

bool Server::isWallRight(QPoint c) {
    for (int i = 0; i < m; i++)
        if ((walls[i][0] == c.x() + 1) && (walls[i][1] == c.y()) && (walls[i][2] == 1))
            return true;

    return false;
}

void Server::dfs(QPoint a) {
    if (w[a.x()][a.y()])
        return;

    w[a.x()][a.y()] = true;

    if (!isWallLeft(a))
        dfs(QPoint(a.x() - 1, a.y()));

    if (!isWallRight(a))
        dfs(QPoint(a.x() + 1, a.y()));

    if (!isWallUp(a))
        dfs(QPoint(a.x(), a.y() - 1));

    if (!isWallDown(a))
        dfs(QPoint(a.x(), a.y() + 1));
}

bool Server::isConnected() {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            w[i][j]= false;

    dfs(QPoint(0, 0));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (!w[i][j])
                return false;

    return true;
}

void Server::generateMap() {
    if (!silence)
        qDebug() << "start generating map";

    m = 0;
    for (int i = 0; i < n; i++) {
        walls[m][0] = 0;
        walls[m][1] = i;
        walls[m++][2] = 1;

        walls[m][0] = n;
        walls[m][1] = i;
        walls[m++][2] = 1;

        walls[m][0] = i;
        walls[m][1] = 0;
        walls[m++][2] = 0;

        walls[m][0] = i;
        walls[m][1] = n;
        walls[m++][2] = 0;
    }

    qsrand(QDateTime::currentDateTime().toMSecsSinceEpoch());

    for (int i = 0; i < 3000; i++) {
        if (i % 300 == 0)
            if (!silence)
                qDebug() << i / 300 * 10 << "%";

        int x = qrand() % n;
        int y = qrand() % n;
        int type = qrand() % 2;

        walls[m][0] = x;
        walls[m][1] = y;
        walls[m][2] = type;
        bool b = false;
        for (int i = 0; i < m; i++)
            if ((walls[i][0] == walls[m][0]) &&
                    (walls[i][1] == walls[m][1]) &&
                        (walls[i][2] == walls[m][2])) {
                            b = true;
                            break;
                        }
        if (b)
            continue;
        m++;

        if (!isConnected())
            m--;
    }

    numberArsenals = n / 4 + 1;
    for (int i = 0; i < numberArsenals; i++)
        arsenal[i] = getFreePoint();

    hospital = getFreePoint();

    walls[n * 4 - 1][0] = -100000;
    walls[n * 4 - 1][1] = -100000;

    qDebug() << "map generated";
}

QPoint Server::getFreePoint() {
    QPoint res;
    res.setX(qrand() % n);
    res.setY(qrand() % n);

    for (int i = 0; i < numberArsenals; i++)
        if ((arsenal[i].x() == res.x()) && (arsenal[i].y() == res.y()))
            return getFreePoint();

    if (hospital == res)
        return getFreePoint();

    return res;
}

int Server::scanInt(QTcpSocket *socket) {
    QString s = socket->readLine();
    s = s.left(s.length() - 1);
    return s.toInt();
}

void Server::forAllClients(QString s) {
    emit forAllClientsPrint(s);
}

QByteArray *Server::generateFieldMessage() {
    QByteArray *result = new QByteArray;
    result->append(QString("field\n"));
    result->append(QString::number(n) + QString("\n"));
    result->append(QString::number(m) + QString("\n"));
    for (int i = 0; i < m; i++)
        for (int j = 0; j < 3; j++)
            result->append(QString::number(walls[i][j]) + QString("\n"));

    return result;
}

QByteArray *Server::generateHeroMessage() {
    QByteArray *result = new QByteArray;
    result->append(QString("hero\n"));
    result->append(QString::number(alreadyPlayers) + "\n");
    for (QMap<int, Player *>::Iterator i = r.begin(); i != r.end(); i++)
        result->append(QString::number(i.value()->socket->socketDescriptor()) + "\n" +
                       QString::number(i.value()->coord->x()) + "\n" +
                       QString::number(i.value()->coord->y()) + "\n" +
                       i.value()->name + "\n");

    return result;
}
