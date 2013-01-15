#include <QApplication>
#include <startdialog.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    startDialog start;
    start.app = &a;
    start.show();

    return a.exec();
}
