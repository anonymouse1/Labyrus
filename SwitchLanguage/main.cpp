#include <stdio.h>
#include <QDir>
#include <QSettings>
#include <QLocale>

#ifdef PORTABLE
    const QString settingsFile = "../labyrus.ini";
#else
    const QString settingsFile = QDir::homePath() + "/.labyrus/labyrus.ini";
#endif

void setLanguage(QString s) {
    QSettings settings(settingsFile, QSettings::IniFormat);
    settings.setValue("language", s);
    printf(("Language successfully setted to: " + s + "\n").toLocal8Bit());
}

int main(int argc, char *argv[]) {
    if (argc > 0)
        if (QString(argv[0]).toUpper() == "RU") {
            setLanguage("ru_RU");
            return 0;
        } else if (QString(argv[0]).toUpper() == "EN") {
            setLanguage("en_US");
            return 0;
        } else if ((QString(argv[0]).toUpper() == "-h") || (QString(argv[0]).toUpper() == "--help")) {
            printf("Welcome to labyrus-chooselanguage documentation\n");
            printf("\tUsage:\n");
            printf("\t\tlabyrus-chooselanguage [language] [-h | --help]\n");
            printf("\t\tFor example: labyrus-chooselanguage ru\n");
            return 0;
        }

    printf("Possible languages:\n");
    printf("0: English\n");
    printf("1: Русский\n");
    printf("-------------------\n");
    printf("Enter your number: ");
    int a;
    scanf("%d", &a);
    if (a == 0) {
        setLanguage("en_US");
    } else if (a == 1) {
        setLanguage("ru_RU");
    } else {
        printf("Unknown language...\n");
        printf("Language setted to default system\n");
        setLanguage(QLocale::system().name());
    }
    return 0;
}
