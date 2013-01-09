#ifndef MAINCLASS_H
#define MAINCLASS_H

#include <QCoreApplication>
#include <server.h>
#include <stdio.h>

class MainClass : public QCoreApplication
{
    Q_OBJECT
public:
    explicit MainClass(int argc, char *argv[]);
private:
    Server *server;
    void die(QString s);
signals:
    
public slots:
    
};

#endif // MAINCLASS_H
