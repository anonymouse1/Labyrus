#include "mainclass.h"


MainClass::MainClass(int argc, char *argv[]) : QCoreApplication(argc, argv)
{
    int n = 20;
    int latency = 16;
    int players = 1;
    bool strong = false;
    for (int i = 0; i < argc; i++)
        if ((QString(argv[i]) == "--help") || (QString(argv[i]) == "-h")) {
            printf("Welcome to Labyrus Server manual\n");
            printf("-n --size <int> for settinh size of maze (-n 20)\n");
            printf("-l --latency <int> for setting latency\n");
            printf("-p --players <int> for setting number of players\n");
            printf("-s --strong for setting strong number of players\n");
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
        } else if ((QString(argv[i]) == "-p") || (QString(argv[i]) == "--players")) {
            bool a;
            players = QString(argv[++i]).toInt(&a);
            if (!a)
                die("invalid number");
        } else if ((QString(argv[i]) == "-s") || (QString(argv[i]) == "--strong")) {
            strong = true;
        } else if ((QString(argv[i]) == "-v") || (QString(argv[i]) == "--version")) {
            die("program has no version [alpha]");
        }

    server = new Server(n, latency, players, strong, this);
}

void MainClass::die(QString s) {
    if (s == "")
        exit(0);

    qDebug() << s;
    exit(1);
}
