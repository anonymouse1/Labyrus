#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(ui->start, SIGNAL(clicked()), this, SLOT(start()));
    QObject::connect(ui->allowConnections, SIGNAL(toggled(bool)), ui->widget, SLOT(setVisible(bool)));
    QObject::connect(ui->actionAbout_Qt, SIGNAL(triggered()), this, SLOT(aboutQt()));
    QObject::connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));


    server = new QProcess(this);
    console = new Console;
    serverShutDown = new QTimer;
    serverShutDown->setInterval(1000);


    loadSettings();
}

MainWindow::~MainWindow() {
    server->terminate();
    server->waitForFinished();
    saveSettings();
    delete ui;
}

void MainWindow::start() {
    QStringList attributes;
    attributes << "-n";
    attributes << ui->fieldSize->text();
    attributes << "--height" << QString::number(ui->heightOfField->value());
    attributes << "--latency" << ui->spinBox->text();
    if (ui->cheats->checkState() == Qt::Checked)
        attributes << "--cheats";

    if (ui->radiation->checkState() == Qt::Checked)
        attributes << "--radiation";

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
    console->setMinimumSize(QSize(500, 300));
    console->setMaximumSize(QSize(500, 300));
    console->show();

    loop = new QEventLoop;


    QObject::connect(server, SIGNAL(finished(int)), console, SLOT(deleteLater()));
    QObject::connect(server, SIGNAL(readyReadStandardError()), this, SLOT(serverSaid()));
    QObject::connect(serverShutDown, SIGNAL(timeout()), this, SLOT(checkForShutDown()));
    QObject::connect(server, SIGNAL(readyReadStandardOutput()), loop, SLOT(quit()));
    QObject::connect(server, SIGNAL(finished(int)), loop, SLOT(quit()));

    QProcess killPreviousServer;
    #ifdef Q_OS_LINUX
        killPreviousServer.start("killall", QStringList() << "labyrus-server");
    #else
        killPreviousServer.start("taskkill.exe", QStringList() << "/F" << "/IM" << "labyrus-server.exe");
    #endif

    killPreviousServer.waitForFinished();

    server->start(prefix + "labyrus-server", attributes);
    loop->exec();
    server->setReadChannel(QProcess::StandardOutput);
    QString err = server->readLine();
    if (err.left(13) != "map generated") {
        QMessageBox::critical(this, tr("Fatal error"), err);
    } else
        console->addString("map generated");


    serverShutDown->start();
}

void MainWindow::aboutQt() {
    QMessageBox::aboutQt(this);
}

void MainWindow::about() {
    QMessageBox::about(this, "About Labyrus", tr("Labyrus is crossplatform game written with opengl and QT Written by Vladislav Tyulbashev"));
}

void MainWindow::saveSettings() {
    QSettings s(settingsFile, QSettings::IniFormat);
    s.setValue("serverGuiGeometry", QVariant(saveGeometry()));
    s.setValue("size", QVariant(ui->fieldSize->value()));
    s.setValue("height", QVariant(ui->heightOfField->value()));
    s.setValue("cheats", QVariant(ui->cheats->checkState() == Qt::Checked));
    s.setValue("radiation", QVariant(ui->radiation->checkState() == Qt::Checked));
    s.setValue("allowRemoteConnections", QVariant(ui->allowConnections->checkState() == Qt::Checked));
    s.setValue("strongNumberPlayers", QVariant(ui->strong->checkState() == Qt::Checked));
    s.setValue("numberPlayers", QVariant(ui->numberPlayers->value()));
    s.setValue("latency", QVariant(ui->spinBox->value()));
    s.setValue("consoleGeometry", QVariant(console->saveGeometry()));
}

void MainWindow::loadSettings() {
    QSettings s(settingsFile, QSettings::IniFormat);
    restoreGeometry(s.value("serverGuiGeometry").toByteArray());
    ui->fieldSize->setValue(s.value("size", QVariant(10)).toInt());
    ui->heightOfField->setValue(s.value("height", QVariant(1)).toInt());
    ui->cheats->setChecked(s.value("cheats", QVariant(false)).toBool());
    ui->allowConnections->setChecked(s.value("allowRemoteConnections", QVariant(false)).toBool());
    ui->widget->setVisible(ui->allowConnections->checkState() == Qt::Checked);
    ui->strong->setChecked(s.value("strongNumberPlayers", QVariant(false)).toBool());
    ui->numberPlayers->setValue(s.value("numberPlayers", QVariant(1)).toInt());
    ui->spinBox->setValue(s.value("latency", QVariant(25)).toInt());
    ui->radiation->setChecked(s.value("radiation", QVariant(false)).toBool());
    console->restoreGeometry(s.value("consoleGeometry").toByteArray());
}

void MainWindow::serverSaid() {
    server->setReadChannel(QProcess::StandardError);
    while (server->canReadLine())
        console->addString(server->readLine());
}

void MainWindow::checkForShutDown() {
    if (!console->isVisible()) {
        qDebug() << "shutting down server";
        server->terminate();
        this->close();
    }
}
