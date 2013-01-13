#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QThread>
#include <QNetworkInterface>
#include <QTcpSocket>
#include <QList>
#include <QHostAddress>
#include <player.h>
#include <QSet>
#include <QMap>
#include <QPair>
#include <QPoint>
#include <stdio.h>
#include <QTimer>
#include <QDateTime>

const qint16 port = 7777;

const int dx[4] = {0, 1, 0, -1};
const int dy[4] = {1, 0, -1, 0};
class Player;
class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(int size, int lat, int players, bool strong, QObject *parent = 0);
    void processConnection(Player *player);
    void sendFieldToPlayer(Player *);
    void sendHeroesToPlayer(Player *);
    int alreadyPlayers;
    void runCommand(QString command, Player *player);
    QMap<int, Player *> r;

private:
    void die(QString);

    bool isWallUp(QPoint c);
    bool isWallDown(QPoint c);
    bool isWallLeft(QPoint c);
    bool isWallRight(QPoint c);

    int scanInt(QTcpSocket *);
    int findNap(QString);
    QPoint getFreePoint();

    bool isConnected();
    void generateMap();
    void dfs(QPoint);

    bool w[100][100];

    int latency, numPlayers;
    bool strongNumPlayers;
    bool gameStart;
    int n;
    int m;
    int walls[10000][3]; // 3:    0 - сверху; 1 - слева
    QPoint hospital;
    QSet<QString> names;

    int numberArsenals;
    QPoint arsenal[100];
protected:
    void incomingConnection(int handle);

signals:
    void sendFields();
    void forAllClientsPrint(QString);
};

#endif // SERVER_H
