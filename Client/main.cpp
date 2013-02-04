#include <QApplication>
#include <startdialog.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    startDialog start(&a, argc, argv);
    start.show();

    return a.exec();
}
