#include "server.h"

Server::Server(QObject *parent) :
    QTcpServer(parent)
{
    if (!listen(QHostAddress::Any, port))
        die(errorString());

    QList<QHostAddress> l = QNetworkInterface::allAddresses();
    for (int i = 0; i < l.size(); i++)
        if (l[i].toIPv4Address() && l[i] != QHostAddress::LocalHost)
            qDebug() << QString("server is upped on ") + QString(l[i].toString()) + QChar(':') + QString::number(port);

//    QObject::connect(this, SIGNAL(newConnection()), this, SLOT(newConnection()));
    alreadyPlayers = 0;

    n = 20;
    m = 0;
    /*walls[0][0] = 1;
    walls[0][1] = 1;
    walls[0][2] = 0;
    walls[1][0] = 1;
    walls[1][1] = 1;
    walls[1][2] = 1;*/
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
    generateMap();

    gameStart = false;
}

void Server::die(QString s) {
    qDebug() << s;
    exit(0);
}

void Server::incomingConnection(int handle) {
    qDebug() << "new connection detected " << handle;
    Player *player = new Player();
    player->socketDescriptor = handle;
    player->server = this;

    player->start();
}

void Server::processConnection(Player *player) {
//    QTcpSocket *socket = server->nextPendingConnection();
    QTcpSocket *socket = player->socket;

    if (!socket->bytesAvailable())
        socket->waitForReadyRead(1000);

    if (socket->readLine() != "Hello maze\n") {
        qDebug() << "this was unknown hacker";
        socket->write("Are you a hacker?");
        socket->disconnectFromHost();
        return;
    }
    socket->waitForReadyRead(1000);
    player->name = socket->readLine();
    for (QMap<int, Player *>::Iterator i = r.begin(); i != r.end(); i++)
        if (i.value()->name == player->name) {
            qDebug() << "used login";
            socket->write("login is already in use");
            socket->flush();
            socket->disconnectFromHost();
            player->deleteLater();
            player->terminate();
            return;
        }


    socket->write("success\n");
    socket->write((QString::number(socket->socketDescriptor()) + "\n").toAscii());
    socket->flush();

    player->socket = socket;
    player->socket->moveToThread(player);
    player->patrons = 3;
    player->walls = 1;
    player->alive = true;
    player->destiny = 0;
    player->destroy = 3;
    player->coord = new QPoint(qrand() % n, qrand() % n);

    qDebug() << player->name << "connected";
    qDebug() << "as" << socket->socketDescriptor();
    r[socket->socketDescriptor()] = player;
    alreadyPlayers++;

    if (alreadyPlayers >= 1) {
        qDebug() << "starting game";
        gameStart = true;
        forAllClientsPrint("gameStart");
        sendFields();
    }
}

void Server::runCommand(QString command, Player *player) {
    if (command[0] == '0')
        player->coord->setY(player->coord->y() - 1);
    else if (command[0] == '1')
        player->coord->setX(player->coord->x() + 1);
    else if (command[0] == '2')
        player->coord->setY(player->coord->y() + 1);
    else if (command[0] == '3')
        player->coord->setX(player->coord->x() - 1);
    else if (command[0] == 'c')
        player->destiny = player->socket->readLine().toInt();
    else if (command[0] == 'a') {
        player->destroy += 1;
        player->walls += 1;
        player->patrons += 3;
        int tmp = player->socket->readLine().left(1).toInt();
        arsenal[tmp] = QPoint(-100, -100);
    } else if (command[0] == 'l') {
        player->alive = true;
    } else if (command[0] == 'p') {
//        player->patrons--;
        player->socket->waitForReadyRead(100);
        QString s = player->socket->readLine();
        qDebug() << "SIGKILL" << s.left(s.length() - 1);
        r[s.left(s.length() - 1).toInt()]->alive = false;
        sendFields();
    } else if (command[0] == 'b') {
        player->walls--;
        walls[m][0] = scanInt(player->socket);
        walls[m][1] = scanInt(player->socket);
        walls[m][2] = scanInt(player->socket);
        m++;
    } else if (command[0] == 'p') {
        player->patrons--;
    } else if (command[0] == 'e') {
        walls[scanInt(player->socket)][0] = -1000;
    } else if (command[0] == 'r') {
        player->destroy--;
    }

    sendHeroes();
}

void Server::sendFieldToPlayer(Player *player) {
    if (!gameStart)
        return;

    qDebug() << this->thread();

    player->sendingInformation.lock();
    QTcpSocket *socket = player->socket;

    socket->write(QString("field\n").toAscii());
    socket->write((QString::number(n) + QString("\n")).toAscii());
    socket->write((QString::number(m) + QString("\n")).toAscii());
    for (int i = 0; i < m; i++)
        for (int j = 0; j < 3; j++)
            socket->write((QString::number(walls[i][j]) + QString("\n")).toAscii());

    //hospital
    socket->write((QString::number(hospital.x()) + "\n" + QString::number(hospital.y()) + "\n").toAscii());
    //arsenals
    socket->write((QString::number(numberArsenals) + "\n").toAscii());
    for (int i = 0; i < numberArsenals; i++)
        socket->write((QString::number(arsenal[i].x()) + "\n" + QString::number(arsenal[i].y()) + "\n").toAscii());

    socket->flush();
    player->sendingInformation.unlock();
}

void Server::sendHeroesToPlayer(Player *player) {
    player->sendingInformation.lock();

    QTcpSocket *socket = player->socket;
    socket->write("hero\n");
    socket->write((QString::number(r.size()) + "\n").toAscii());
    for (QMap<int, Player *>::Iterator i = r.begin(); i != r.end(); i++) {
        socket->write((QString::number(i.value()->socket->socketDescriptor()) + "\n" +
                       QString::number(i.value()->coord->x()) + "\n" +
                       QString::number(i.value()->coord->y()) + "\n" +
                       QString::number(i.value()->alive) + "\n" +
                       QString::number(i.value()->patrons) + "\n" +
                       QString::number(i.value()->walls) + "\n" +
                       QString::number(i.value()->destroy) + "\n" +
                       i.value()->name + "\n").toAscii());
        }

    socket->flush();
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

    QPoint p(0, 0);
    dfs(p);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (!w[i][j])
                return false;

    return true;
}

void Server::generateMap() {
    qDebug() << "start generating map";
    qsrand(QDateTime::currentDateTime().toMSecsSinceEpoch());

    for (int i = 0; i < 3000; i++) {
        if (i % 300 == 0)
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

    int a = qrand() % (4 * n);
    walls[a][0] = -100;
    walls[a][1] = -100;
    walls[a][2] = -100;

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

void Server::forAllClientsPrint(QString s) {
    for (QMap<int, Player *>::Iterator i = r.begin(); i != r.end(); i++) {
        i.value()->socket->write((s + "\n").toAscii());
        i.value()->socket->flush();
    }
}

void Server::sendFields() {
    for (QMap<int, Player *>::Iterator i = r.begin(); i != r.end(); i++)
        sendFieldToPlayer(i.value());
}

void Server::sendHeroes() {
    for (QMap<int, Player *>::Iterator i = r.begin(); i != r.end(); i++)
        sendHeroesToPlayer(i.value());
}
