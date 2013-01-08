#ifndef COMMANDSEND_H
#define COMMANDSEND_H

#include <QTcpSocket>

class CommandSend : public QObject
{
    Q_OBJECT

public:
    explicit CommandSend(QTcpSocket *s, QObject *parent = 0);

    void go(QString, bool flush, bool b = true);
private:
    QTcpSocket *socket;

public slots:

};

#endif // COMMANDSEND_H
