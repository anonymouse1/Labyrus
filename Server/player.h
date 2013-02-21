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
    Q_OBJECT
public:
    explicit Player(int latency, QObject *parent = 0);

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
    int latency;
private slots:
    void sendHeroTime();
    void printString(QString);
    void radiation();
public slots:
    void refreshTime();
    void readyRead();
    void disconnect();
signals:
    void say(QString);
protected:
    void run();

};

#endif // PLAYER_H
