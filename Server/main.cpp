#include <server.h>
#include <mainclass.h>

int main(int argc, char *argv[])
{
    MainClass a(argc, argv);
    return a.exec();
}
