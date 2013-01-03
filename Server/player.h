#ifndef PLAYER_H
#define PLAYER_H

#include <QTcpSocket>
#include <QPoint>
#include <QThread>
#include <server.h>
#include <QMutex>
#include <QTimer>

class Server;
class Player : public QThread
{
public:
    explicit Player(QObject *parent = 0);

    QString name;
    QPoint *coord;
    bool alive;
    int patrons;
    int walls;
    int destiny;
    int destroy;

    QTcpSocket *socket;
    int socketDescriptor;
    Server *server;
    QMutex sendingInformation;
private:
    QTimer *refresh;
private slots:
    void refreshTime();
    void readyRead();
    void disconnect();
protected:
    void run();

};

#endif // PLAYER_H
