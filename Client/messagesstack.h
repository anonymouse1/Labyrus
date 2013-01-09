#ifndef MESSAGESSTACK_H
#define MESSAGESSTACK_H

#include <QObject>
#include <QString>
#include <QList>
#include <QTimer>

class MessagesStack : public QObject
{
    Q_OBJECT
public:
    explicit MessagesStack(QObject *parent = 0);
    void addMessage(QString);
    QList<QString> getMessages();
private:
    QList<QString> list;
private slots:
    void timeToRemove();
signals:
    
public slots:
    
};

#endif // MESSAGESSTACK_H
