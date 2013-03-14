#include <QApplication>
#include <startdialog.h>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    #ifdef PORTABLE
        QString localePrefix = "../translations/labyrus-";
    #else
        QString localePrefix = "/usr/share/labyrus/translations/labyrus-";
    #endif

    QTranslator *trans = new QTranslator;
    trans->load(localePrefix + QLocale::system().name());
    a.installTranslator(trans);

    startDialog start(&a, argc, argv);
    start.show();

    return a.exec();
}
