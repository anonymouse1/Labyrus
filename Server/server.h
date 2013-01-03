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
    explicit Server(QObject *parent = 0);
    void processConnection(Player *player);
    void sendFieldToPlayer(Player *);
    void sendHeroesToPlayer(Player *);
    int alreadyPlayers;
    void runCommand(QString command, Player *player);
    QMap<int, Player *> r;
    void sendHeroes();


private:
    void die(QString);

    bool isWallUp(QPoint c);
    bool isWallDown(QPoint c);
    bool isWallLeft(QPoint c);
    bool isWallRight(QPoint c);

    int scanInt(QTcpSocket *);
    int findNap(QString);
    QPoint getFreePoint();

    void forAllClientsPrint(QString);

    bool isConnected();
    void generateMap();
    void dfs(QPoint);

    bool w[100][100];

    bool gameStart;
    int n;
    int m;
    int walls[10000][3]; // 3:    0 - сверху; 1 - слева
    QPoint hospital;

    int numberArsenals;
    QPoint arsenal[100];
protected:
    void incomingConnection(int handle);

private slots:
    void sendFields();
};

#endif // SERVER_H
