#include "messagesstack.h"

MessagesStack::MessagesStack(QObject *parent): QObject(parent) {
}

QList<QString> MessagesStack::getMessages() {
    return list;
}

void MessagesStack::addMessage(QString s) {
    if (list.size() == 10)
        list.removeFirst();

    if (s[s.length() - 1] == '\n')
        s.remove(s.length() - 1, 1);

    list.append(s);
    if (list.size() != 10) {
        QTimer *timer = new QTimer;
        timer->setInterval(15000);
        QObject::connect(timer, SIGNAL(timeout()), this, SLOT(timeToRemove()));
        QObject::connect(timer, SIGNAL(timeout()), timer, SLOT(deleteLater()));
        timer->start();
    }
}

void MessagesStack::timeToRemove() {
    list.removeFirst();
}
