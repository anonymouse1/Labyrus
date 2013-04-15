#include "server.h"

Server::Server(bool win, qint16 port, bool rad, bool cheat, int size, int height, int lat, int players, bool strong, QObject *parent) :
    QTcpServer(parent)
{
    if (!listen(QHostAddress::Any, port))
        die(errorString());

    alreadyPlayers = 0;
    qDebug() << "port:" << port;
    qDebug() << "win support:" << win;
    qDebug() << "radiation:" << rad;
    qDebug() << "cheats:" << cheat;
    qDebug() << "size:" << size;
    qDebug() << "height:" << height;
    qDebug() << "latency:" << lat;
    qDebug() << "Number of players:" << players;
    qDebug() << "Fixed number of players:" << strong;

    latency = lat;
    numPlayers = players;
    strongNumPlayers = strong;
    cheats = cheat;
    radiation = rad;
    allowWin = win;
    if (radiation) {
        radiationTimer = new QTimer;
        radiationTimer->setInterval(4000);
    }
    n = size;
    h = height;
    QTime generatingMap;
    generatingMap.start();
    generateMap();
    qDebug() << generatingMap.elapsed() << "ms";
    QList<QHostAddress> l = QNetworkInterface::allAddresses();
    for (int i = 0; i < l.size(); i++)
        if (l[i].toIPv4Address() && l[i] != QHostAddress::LocalHost)
            qDebug() << QString("server is upped on ") + QString(l[i].toString()) + QChar(':') + QString::number(port);

    gameStart = false;
}

void Server::die(QString s) {
    printf("%s\n", s.toLocal8Bit().constData());
    exit(0);
}

void Server::incomingConnection(qintptr handle) {
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
    QObject::connect(player, SIGNAL(finished()), player, SLOT(deleteLater()));
    QObject::connect(player, SIGNAL(finished()), this, SLOT(sendWinners()));
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
    socket->write((QString::number(socket->socketDescriptor()) + "\n").toLocal8Bit());
    socket->write((QString::number(latency) + "\n").toLocal8Bit());

    player->socket = socket;
    qDebug() << player->name << "connected";
    qDebug() << "as" << socket->socketDescriptor();
    alreadyPlayers++;

    if ((alreadyPlayers >= numPlayers) || (gameStart)) {
        qDebug() << "starting game";
        if (gameStart)
            socket->write("gameStart\n");
        else {
            gameStart = true;
            emit forAllClientsPrint("gameStart");
        }

        if (cheats) {
            emit forAllClientsPrint("cheats");
            emit forAllClientsPrint("S\nCheats allowed");
        }

        if (radiation) {
            emit forAllClientsPrint("S\nThere is some radiation near");
            radiationTimer->start();
        }

        if (!allowWin) {
            emit forAllClientsPrint("nowin");
            emit forAllClientsPrint("S\nWin support is DISABLED");
        }
    }

    sendFieldToPlayer(player);
    emit forAllClientsPrint("S\n" + player->name + " connected");
}

void Server::runCommand(QString command, Player *player) {
    if (command[0] == 'n') {
        QString s;
        s = player->socket->readLine();
        player->coord.x = s.left(s.length() - 1).toDouble();
        s = player->socket->readLine();
        player->coord.y = s.left(s.length() - 1).toDouble();
        s = player->socket->readLine();
        player->coord.h = s.left(s.length() - 1).toDouble();
        s = player->socket->readLine();
        player->progress = s.left(s.length() - 1).toInt();
    } else if (command[0] == 'I') {
        forAllClientsPrint("S\n" + player->name + ": " + QString::fromLocal8Bit(player->socket->readLine()));
    } else if (command[0] == 'v') {
        player->setValid();
    } else if (command[0] == 'p') {
        player->socket->write("p\n");
    } else if (command[0] == 'w') {
        winners.push_back(player->name);
        qDebug() << winners;
        sendWinners();
        forAllClients("S\n" + player->name + " finished (" + QString::number(winners.size()) + " place)");

        if (winners.size() == alreadyPlayers) {
            forAllClients("restart");
            forAllClients("S\nGame finished\nS\nPlease wait...");
            generateMap();
            emit sendFields();
            forAllClients("S\nMap generated");
            emit forAllClientsPrint("gameStart");
            winners.clear();
        }
    }

    if (player->socket->canReadLine())
        runCommand(player->socket->readLine(), player);
}

void Server::sendFieldToPlayer(Player *player, QByteArray *data) {
    qDebug() << "sending field to player" << player->name << QTime::currentTime();
    if (data == NULL)
        data = generateFieldMessage();

    player->printString(*data);
}

void Server::sendHeroesToPlayer(Player *player, QByteArray *data) {
    if (data == NULL)
        data = generateHeroMessage();

    player->printString(*data);
}

bool Server::isWallDown(int x, int y, int k) {
    for (int i = 0; i < m; i ++)
        if ((walls[i][0] == x) && (walls[i][1] == y) && (walls[i][2] == k) && (walls[i][3] == 2))
            return true;

    return false;
}

bool Server::isWallUp(int x, int y, int k) {
    for (int i = 0; i < m; i++)
        if ((walls[i][0] == x) && (walls[i][1] == y) && (walls[i][2] == k + 1) && (walls[i][3] == 2))
            return true;

    return false;
}

bool Server::isWallLeft(int x, int y, int k) {
    for (int i = 0; i < m; i++)
        if ((walls[i][0] == x) && (walls[i][1] == y) && (walls[i][2] == k) && walls[i][3] == 1)
            return true;

    return false;
}

bool Server::isWallRight(int x, int y, int k) {
    for (int i = 0; i < m; i++)
        if ((walls[i][0] == x + 1) && (walls[i][1] == y) && (walls[i][2] == k) && (walls[i][3] == 1))
            return true;

    return false;
}

bool Server::isWallForward(int x, int y, int k) {
    for (int i = 0; i < m; i++)
        if ((walls[i][0] == x) && (walls[i][1] == y + 1) && (walls[i][2] == k) && (walls[i][3] == 0))
            return true;

    return false;
}

bool Server::isWallBackward(int x, int y, int k) {
    for (int i = 0; i < m; i++)
        if ((walls[i][0] == x) && (walls[i][1] == y) && (walls[i][2] == k) && (walls[i][3] == 0))
            return true;

    return false;
}

void Server::dfs(int x, int y, int k) {
    if (w[x][y][k])
        return;

    w[x][y][k] = true;

    if (!isWallLeft(x, y, k))
        dfs(x - 1, y, k);

    if (!isWallRight(x, y, k))
        dfs(x + 1, y, k);

    if (!isWallForward(x, y, k))
        dfs(x, y + 1, k);

    if (!isWallBackward(x, y, k))
        dfs(x, y - 1, k);

    if (!isWallUp(x, y, k))
        dfs(x, y, k + 1);

    if (!isWallDown(x, y, k))
        dfs(x, y, k - 1);
}

bool Server::isConnected() {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k <  h; k++)
                w[i][j][k] = false;

    dfs(0, 0, 0);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < h; k++)
                if (!w[i][j][k])
                    return false;

    return true;
}

void Server::generateMap() {
    qDebug() << "start generating map";

    m = 0;
    for (int j = 0; j < h; j++)
        for (int i = 0; i < n; i++) {
            walls[m][0] = 0;
            walls[m][1] = i;
            walls[m][2] = j;
            walls[m++][3] = 1;

            walls[m][0] = n;
            walls[m][1] = i;
            walls[m][2] = j;
            walls[m++][3] = 1;

            walls[m][0] = i;
            walls[m][1] = 0;
            walls[m][2] = j;
            walls[m++][3] = 0;

            walls[m][0] = i;
            walls[m][1] = n;
            walls[m][2] = j;
            walls[m++][3] = 0;
        }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            walls[m][0] = i;
            walls[m][1] = j;
            walls[m][2] = 0;
            walls[m++][3] = 2;

            walls[m][0] = i;
            walls[m][1] = j;
            walls[m][2] = h;
            walls[m++][3] = 2;
        }

    qsrand(QDateTime::currentDateTime().toMSecsSinceEpoch());

    int tmpWalls[n * n * h * 3][4];
    int cur = 0;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < h; k++)
                for (int z = 0; z < 3; z++) {
                    tmpWalls[cur][0] = i;
                    tmpWalls[cur][1] = j;
                    tmpWalls[cur][2] = k;
                    tmpWalls[cur][3] = z;
                    cur++;
                }

    for (int i = 1; i < cur; i++) {
        int j = qrand() % i;
        for (int k = 0; k < 4; k++)
            swap(tmpWalls[i][k], tmpWalls[j][k]);
    }

    for (int i = 0; i < cur; i++) {
        if (i % (cur / 10 + 1) == 0)
            qDebug() << i / (cur / 10 + 1) * 10 << "%";

        for (int k = 0; k < 4; k++)
            walls[m][k] = tmpWalls[i][k];

        bool b = false;
        for (int i = 0; i < m; i++)
            if ((walls[i][0] == walls[m][0]) &&
                    (walls[i][1] == walls[m][1]) &&
                        (walls[i][2] == walls[m][2]) &&
                            (walls[i][3] == walls[m][3])) {
                                b = true;
                                break;
                            }
        if (b)
            continue;
        m++;

        if (!isConnected())
            m--;
    }

    walls[n * 4 - 1][0] = -100000;
    walls[n * 4 - 1][1] = -100000;

    printf("map generated\n");
    fflush(stdout);
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
    result->append(QString::number(h) + QString("\n"));
    result->append(QString::number(m) + QString("\n"));
    for (int i = 0; i < m; i++)
        for (int j = 0; j < 4; j++)
            result->append(QString::number(walls[i][j]) + QString("\n"));

    return result;
}

QByteArray *Server::generateHeroMessage() {
    QByteArray *result = new QByteArray;
    result->append(QString("hero\n"));
    result->append(QString::number(alreadyPlayers) + "\n");
    for (QMap<int, Player *>::Iterator i = r.begin(); i != r.end(); i++)
        result->append(QString::number(i.value()->socket->socketDescriptor()) + "\n" +
                       QString::number(i.value()->coord.x) + "\n" +
                       QString::number(i.value()->coord.y) + "\n" +
                       QString::number(i.value()->coord.h) + "\n" +
                       QString::number(i.value()->progress) + "\n" +
                       i.value()->name + "\n");

    return result;
}

void Server::timeToDie() {
    //            forAllClientsPrint("f");
    exit(0);
}

void Server::sendWinners() {
    QByteArray win;
    win += "w\n";
    win += QString::number(winners.size());
    for (int i = 0; i < winners.size(); i++)
        win += "\n" + winners[i];

    forAllClients(win);
}
