#ifndef PLAYER_H
#define PLAYER_H

#include <QTcpSocket>
#include <QPoint>
#include <QThread>
#include <server.h>
#include <QMutex>
#include <QTimer>

const int lacency = 5000; //ms

class Server;
class Player : public QThread
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = 0);

    QString name;
    QPointF *coord;
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
    QTimer *sendHeroesTime;
private slots:
    void sendHeroTime();
public slots:
    void refreshTime();
    void readyRead();
    void disconnect();
protected:
    void run();

};

#endif // PLAYER_H
