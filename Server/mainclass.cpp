#include "mainclass.h"


MainClass::MainClass(int argc, char *argv[]) : QCoreApplication(argc, argv)
{
    int n = 20;
    int latency = 16;
    for (int i = 0; i < argc; i++)
        if ((QString(argv[i]) == "--help") || (QString(argv[i]) == "-h")) {
            printf("Welcome to Labyrus Server manual\n");
            printf("-n --size <int> for settinh size of maze (-n 20)\n");
            printf("-l --latency <int> for setting latency\n");
            printf("-h --help to view this help\n");
            printf("-v --version to view version\n");
            die("");
        } else if ((QString(argv[i]) == "-n") || (QString(argv[i]) == "--size")) {
            bool a;
            n = QString(argv[++i]).toInt(&a);
            if (!a)
                die("invalid number");
        } else if ((QString(argv[i]) == "-l") || (QString(argv[i]) == "--latency")) {
            bool a;
            latency = QString(argv[++i]).toInt(&a);
            if (!a)
                die("invalid number");
        } else if ((QString(argv[i]) == "-v") || (QString(argv[i]) == "--version")) {
            die("program has no version [alpha]");
        }

    server = new Server(n, latency, this);
}

void MainClass::die(QString s) {
    if (s == "")
        exit(0);

    qDebug() << s;
    exit(1);
}
