#ifndef PLAYER_H
#define PLAYER_H

#include <QTcpSocket>
#include <QPoint>
#include <QThread>
#include <server.h>
#include <QMutex>
#include <QTimer>

struct fpoint {
    double x;
    double y;
    double h;
};

class Server;
class Player : public QThread
{
    Q_OBJECT
public:
    explicit Player(int latency, QObject *parent = 0);

    QString name;
    fpoint coord;
    int progress;
    QTcpSocket *socket;
    int socketDescriptor;
    Server *server;
    QMutex sendingInformation;
    void flush();
private:
    QTimer *refresh;
    QTimer *sendHeroesTime;
    QTimer *checkValid;
    int latency;
private slots:
    void sendHeroTime();
    void radiation();
    void lagger();
public slots:
    void refreshTime();
    void readyRead();
    void disconnect();
    void setValid();
    void printString(QString);
signals:
    void say(QString);
protected:
    void run();

};

#endif // PLAYER_H
