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

    #ifdef PORTABLE
        prefix = "./";
    #else
        prefix = "/usr/bin/";
    #endif

    ui->widget->hide();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::start() {
    QStringList attributes;
    attributes << "--silence";
    attributes << "-n";
    attributes << ui->fieldSize->text();
    attributes << "--latency" << ui->spinBox->text();
    if (ui->cheats->checkState() == Qt::Checked)
        attributes << "--cheats";

    if (ui->allowConnections->checkState() == Qt::Checked) {
        attributes << "-p";
        attributes << ui->numberPlayers->text();
        if (ui->strong->checkState() == Qt::Checked)
            attributes << "--strong";
    } else {
        attributes << "-p";
        attributes << "1";
        attributes << "--strong";
    }
    this->hide();
    QProcess *server = new QProcess;
    QProcess *client = new QProcess;
    QEventLoop *loop = new QEventLoop;
    QTimer *timeout = new QTimer;

    QObject::connect(timeout, SIGNAL(timeout()), loop, SLOT(quit()));
    timeout->setInterval(7000);
    timeout->start();


    QObject::connect(client, SIGNAL(finished(int)), server, SLOT(terminate()));
    QObject::connect(server, SIGNAL(readyRead()), loop, SLOT(quit()));
    QObject::connect(server, SIGNAL(finished(int)), loop, SLOT(quit()));
    QObject::connect(server, SIGNAL(finished(int)), this, SLOT(close()));
    server->setReadChannel(QProcess::StandardError);
    server->start(prefix + "labyrus-server", attributes);
    loop->exec();

    qDebug() << server->canReadLine();
    QString res = server->readLine();
    if (res != "map generated \n")
        QMessageBox::about(this, QString("Die"), res);

    attributes.clear();
    attributes << "-n" << ui->name->text();
    attributes << "-p" << "7777";
    attributes << "-i" << "127.0.0.1" ;
    attributes << "--start";
    client->start(prefix + "labyrus-client", attributes);
}
