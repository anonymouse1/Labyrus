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

const int dx[4] = {0, 1, 0, -1};
const int dy[4] = {1, 0, -1, 0};
class Player;
class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(qint16 port, bool rad, bool cheat, bool sil, int size, int height, int lat, int players, bool strong, QObject *parent = 0);
    void processConnection(Player *player);
    void sendFieldToPlayer(Player *player, QByteArray *data = NULL);
    void sendHeroesToPlayer(Player *player, QByteArray *data = NULL);
    int alreadyPlayers;
    void runCommand(QString command, Player *player);
    QMap<int, Player *> r;
    QSet<QString> names;
    QTimer *radiationTimer;
    bool radiation;

private:
    void die(QString);

    bool isWallUp(int x, int y, int k);
    bool isWallDown(int x, int y, int k);
    bool isWallLeft(int x, int y, int k);
    bool isWallRight(int x, int y, int k);
    bool isWallForward(int x, int y, int k);
    bool isWallBackward(int x, int y, int k);

    int scanInt(QTcpSocket *);
    int findNap(QString);
    QPoint getFreePoint();
    QByteArray *generateFieldMessage();
    QByteArray *generateHeroMessage();

    bool isConnected();
    void generateMap();
    void dfs(int x, int y, int k);

    bool w[100][100][100];

    int latency, numPlayers;
    bool strongNumPlayers;
    bool gameStart;
    bool cheats;
    int n;
    int h;
    int m;
    int walls[10000][4]; // 3:    0 - сверху; 1 - слева
    QPoint hospital;
    bool silence;

    int numberArsenals;
    QPoint arsenal[100];
private slots:
    void forAllClients(QString);
protected:
    void incomingConnection(qintptr handle);

signals:
    void sendFields();
    void forAllClientsPrint(QString);
};

#endif // SERVER_H
