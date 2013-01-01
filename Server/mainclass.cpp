#include "mainclass.h"


mainClass::mainClass(int argc, char *argv[]) : QCoreApplication(argc, argv)
{
    server = new Server;
}
