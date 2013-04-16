#include "mainclass.h"


MainClass::MainClass(int argc, char *argv[]) : QCoreApplication(argc, argv)
{
    int n = 4;
    int h = 4;
    int latency = 50;
    int players = 1;
    qint16 port = 7777;
    bool strong = false;
    bool debug = false;
    bool radiation = false;
    for (int i = 0; i < argc; i++)
        if ((QString(argv[i]) == "--help") || (QString(argv[i]) == "-h")) {
            printf("\tWelcome to Labyrus Server manual\n");
            printf("\tFor all questions please contact vladislav.tyulbashev@yandex.ru\n");
            printf("\t\t-n --size <int> for settin size of maze (-n 20)\n");
            printf("\t\t-m --height <int> for setting height of maze\n");
            printf("\t\t-d --debug for debug mode\n");
            printf("\t\t-l --latency <int> for setting latency\n");
            printf("\t\t-p --players <int> for setting number of players\n");
            printf("\t\t-s --strong for setting strong number of players\n");
            printf("\t\t-r --radiation for enable radiation\n");
            printf("\t\t-t --port <int> for setting listening port\n");
            printf("\t\t-h --help to view this help\n");
            printf("\t\t-v --version to view version\n");
            die("");
        } else if ((QString(argv[i]) == "-n") || (QString(argv[i]) == "--size")) {
            bool a;
            n = QString(argv[++i]).toInt(&a);
            if (!a)
                die("invalid number");
        } else if ((QString(argv[i]) == "-m") || (QString(argv[i]) == "--height")) {
            bool a;
            h = QString(argv[++i]).toInt(&a);
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
        } else if ((QString(argv[i]) == "-r") || (QString(argv[i]) == "--radiation"))
            radiation = true;
          else if ((QString(argv[i]) == "-d") || (QString(argv[i]) == "--debug"))
            debug = true;
          else if ((QString(argv[i]) == "-t") || (QString(argv[i]) == "--port")) {
            bool a;
            port = QString(argv[++i]).toInt(&a);
            if (!a)
                die("invalid number");
        }

    if (n * n * h > 3000)
        die("The maze is too big");
    server = new Server(debug, port, radiation, n, h, latency, players, strong, this);
}

void MainClass::die(QString s) {
    if (s != "")
        qDebug() << s;
    #ifdef Q_OS_LINUX
        if (s == "")
            quick_exit(0);
        else
            quick_exit(1);
    #else
        exit(1);
    #endif
}
