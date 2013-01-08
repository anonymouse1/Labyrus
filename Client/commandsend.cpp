#include "commandsend.h"

CommandSend::CommandSend(QTcpSocket *s, QObject *parent) :
    QObject(parent)
{
    socket = s;
}

void CommandSend::go(QString s, bool flush, bool b) {
    if (b)
        s += "\n";
    socket->write(s.toAscii());
    if (flush)
        socket->flush(); // it was a many strange bugs, was not it?
}
