#ifndef NETWORKCLASS_H
#define NETWORKCLASS_H

#include <QObject>
#include <QPointF>
#include <QString>
#include <QPoint>
#include <QMutex>
#include <QTcpSocket>
#include <QMessageBox>
#include <QTimer>
#include <QTime>
#include <QThread>
#include <QHostAddress>
#include <messagesstack.h>
#include <math.h>

struct fpoint {
    double x;
    double y;
    double h;

    const inline double rast(const fpoint &a) { return sqrt((x - a.x) * (x - a.x) + (y - a.y) * (y - a.y) + (h - a.h) * (h - a.h)); }
};

struct gpoint {
    int x;
    int y;
    int h;
};

class NetworkClass : public QThread
{
    Q_OBJECT
public:
    explicit NetworkClass(QHostAddress ip, quint16 port, QString myName, QThread *parent = 0);
    int walls[10000][4];
    int n, m, h;
    fpoint coord;
    int otherHeroes;
    fpoint heroes[100];
    QString heroNames[100];
    int otherAlive[100];
    int patrons;
    int wall;
    int destroy;
    bool alive;
    bool cheats;
    bool radiation;
    int myDescriptor;
    int descriptors[100];
    double angle;
    double yAngle;
    QString login;
    MessagesStack *messages;
    QTime *pingTime;
    bool escapeMode;

    int latency;

    void checkAngles();
    int getFloor();
    void go(QString s, bool flush = false, bool addEndLine = true);
private:
    QMutex processingInformation;
    QTcpSocket *mainSocket;
    QTimer *failConnectionTime;
    QTimer *pingTimer;
    QTimer *serverRefresh;
    QDataStream *stream;
    bool fullRefresh;

    QHostAddress targetIp;
    quint16 targetPort;

    double scanInt();
    void readField();
    void readHeroes();
protected:
    void run();

public slots:
    void readInformation();
    void runCommand(QString);

private slots:
    void connectionEstablished();
    void failConnection();
    void ping();
    void refreshCoords();

signals:
    void successConnection();
    void connectionFailed();
    void gameStart();
    
};

#endif // NETWORKCLASS_H
