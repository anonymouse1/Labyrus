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

const int lacency = 100; // ms

class NetworkClass : public QThread
{
    Q_OBJECT
public:
    explicit NetworkClass(QHostAddress ip, quint16 port, QString myName, QThread *parent = 0);
    int walls[10000][3];
    int n, m;
    QPointF coord;
    int otherHeroes;
    QPointF heroes[100];
    int numberArsenals;
    QPoint arsenal[100];
    QString heroNames[100];
    int otherAlive[100];
    int patrons;
    int wall;
    int destroy;
    bool alive;
    QPoint hospital;
    int myDescriptor;
    int descriptors[100];
    double angle;
    double yAngle;
    QString login;
    MessagesStack *messages;

    void processInformation();
    void go(QString s, bool flush = false, bool addEndLine = true);
private:
    QMutex processingInformation;
    QTcpSocket *mainSocket;
    QTimer *failConnectionTime;
    bool fullRefresh;

    QHostAddress targetIp;
    quint16 targetPort;

    double scanInt();
    void readField();
    void readHeroes();
protected:
    void run();

private slots:
    void readInformation();
    void connectionEstablished();
    void failConnection();

signals:
    void successConnection();
    void connectionFailed();
    void gameStart();
public slots:
    
};

#endif // NETWORKCLASS_H
