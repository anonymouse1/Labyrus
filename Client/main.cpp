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

    QSettings *language = new QSettings(settingsFile, QSettings::IniFormat);
    QString locale = localePrefix + language->value("language", QVariant(QLocale::system().name())).toString();

    QTranslator *trans = new QTranslator;
    trans->load(locale);
    a.installTranslator(trans);

    delete language;


    startDialog start(&a, argc, argv);
    start.show();

    return a.exec();
}
