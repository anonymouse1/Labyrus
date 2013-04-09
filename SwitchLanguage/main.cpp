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
    if (argc > 1) {
        if (QString(argv[1]).toUpper() == "RU") {
            setLanguage("ru_RU");
            return 0;
        } else if (QString(argv[1]).toUpper() == "EN") {
            setLanguage("en_US");
            return 0;
        } else if ((QString(argv[1]).toUpper() == "-H") || (QString(argv[1]).toUpper() == "--HELP")) {
            printf("Welcome to labyrus-chooselanguage documentation\n");
            printf("\tUsage:\n");
            printf("\t\tlabyrus-switchlanguage [language] [-h | --help]\n");
            printf("\t\tFor example: labyrus-switchlanguage ru\n");
            return 0;
        } else {
            printf("\tUnknown arguments.\n");
            printf("\tUse -h or --help for help\n");
            return 0;
        }
    }

    printf("Possible languages:\n");
    printf("0: English\n");
    printf("1: Russian\n");
    printf("-------------------\n");
    printf("Enter your number: ");
    int a;
    while (scanf("%d", &a) != 1) {
        printf("Damaged integer\n");
        printf("Please try again\n");
    }

    if (a == 0) {
        setLanguage("en_US");
    } else if (a == 1) {
        setLanguage("ru_RU");
    } else {
        printf("Unknown language...\n");
        printf("Setting to default system\n");
        setLanguage(QLocale::system().name());
    }
    return 0;
}
