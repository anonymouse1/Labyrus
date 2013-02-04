#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(ui->start, SIGNAL(clicked()), this, SLOT(start()));
    QObject::connect(ui->allowConnections, SIGNAL(toggled(bool)), ui->widget, SLOT(setShown(bool)));

    ui->widget->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::start() {
    QStringList attributes;
    attributes << "-n";
    attributes << ui->fieldSize->text();
    attributes << "--latency" << ui->spinBox->text();
    if (ui->allowConnections) {
        attributes << "-p";
        attributes << ui->numberPlayers->text();
        if (ui->strong)
            attributes << "--strong";
    } else {
        attributes << "-p";
        attributes << "1";
        attributes << "--strong";
    }
    this->hide();
    QProcess *server = new QProcess;
    QObject::connect(this, SIGNAL(destroyed()), server, SLOT(terminate()));
    server->start("/usr/bin/labyrus-server", attributes);

    QEventLoop *loop = new QEventLoop;
    QTimer::singleShot(5000, loop, SLOT(quit()));
    loop->exec();

    attributes.clear();
    attributes << "-n" << ui->name->text();
    attributes << "-p" << "7777";
    attributes << "-i" << "127.0.0.1" ;
    attributes << "--start";
    QProcess *client = new QProcess;
    QObject::connect(client, SIGNAL(finished(int)), this, SLOT(close()));
    client->start("/usr/bin/labyrus-client", attributes);
}
