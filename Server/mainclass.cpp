#include "mainclass.h"


MainClass::MainClass(int argc, char *argv[]) : QCoreApplication(argc, argv)
{
    int n = 20;
    int latency = 50;
    int players = 1;
    bool strong = false;
    bool silence = false;
    bool cheats = false;
    bool radiation = false;
    for (int i = 0; i < argc; i++)
        if ((QString(argv[i]) == "--help") || (QString(argv[i]) == "-h")) {
            printf("\tWelcome to Labyrus Server manual\n");
            printf("\tFor all questions please contact vladislav.tyulbashev@yandex.ru\n");
            printf("\t\t-n --size <int> for settin size of maze (-n 20)\n");
            printf("\t\t-l --latency <int> for setting latency\n");
            printf("\t\t-p --players <int> for setting number of players\n");
            printf("\t\t-s --strong for setting strong number of players\n");
            printf("\t\t-i --silence for more silence\n");
            printf("\t\t-t --radiation for enable radiation\n");
            printf("\t\t-c --cheats for allow cheats\n");
            printf("\t\t-h --help to view this help\n");
            printf("\t\t-v --version to view version\n");
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
        } else if ((QString(argv[i]) == "-i") || (QString(argv[i]) == "--silence"))
            silence = true;
          else if ((QString(argv[i]) == "-r") || (QString(argv[i]) == "--radiation"))
            radiation = true;
          else if ((QString(argv[i]) == "-c") || (QString(argv[i]) == "--cheats"))
            cheats = true;

    server = new Server(radiation, cheats, silence, n, latency, players, strong, this);
}

void MainClass::die(QString s) {
    if (s == "")
        quick_exit(0);

    qDebug() << s;
    quick_exit(1);
}
