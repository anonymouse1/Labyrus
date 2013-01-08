#ifndef COMMANDSEND_H
#define COMMANDSEND_H

#include <QWidget>
#include <ui_command.h>
#include <QTcpSocket>

class CommandSend : public QWidget
{
    Q_OBJECT

public:
    explicit CommandSend(QTcpSocket *s, QWidget *parent = 0);
    Ui::commands *ui;

    void go(QString, bool flush, bool b = true);
    void wasPrinted(QString);
private:
    QTcpSocket *socket;

public slots:
    void print();

signals:
    void startBot();

public slots:

};

#endif // COMMANDSEND_H
