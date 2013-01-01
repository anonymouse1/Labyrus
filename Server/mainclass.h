#ifndef MAINCLASS_H
#define MAINCLASS_H

#include <QCoreApplication>
#include <server.h>

class mainClass : public QCoreApplication
{
    Q_OBJECT
public:
    explicit mainClass(int argc, char *argv[]);
private:
    Server *server;
signals:
    
public slots:
    
};

#endif // MAINCLASS_H
