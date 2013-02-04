#include <startdialog.h>

startDialog::startDialog(QApplication *a, int argc, char *argv[], QWidget *parent) : QWidget(parent) {
    ui = new Ui::Form;
    ui->setupUi(this);
    app = a;
    bool st = false;
    for (int i = 0; i < argc; i++) {
        QString cur = argv[i];
        if ((cur == "-i") || (cur == "--ip"))
            ui->lineEdit_2->setText(argv[i + 1]);
        else if ((cur == "-p") || (cur == "--port"))
            ui->spinBox->setValue(QString(argv[i + 1]).toInt());
        else if ((cur == "-n") || (cur == "--name"))
            ui->lineEdit->setText(QString(argv[i + 1]));
        else if ((cur == "s") || (cur == "--start"))
            st = true;
        else if ((cur == "-h") || (cur == "--help")) {
            printf("Welcome to labyrus-singleplayer documentation\n");
            printf("-i --ip int.int.int.int        to set ip address\n");
            printf("-p --port int                  to set port number\n");
            printf("-n --name name                 to set name of player\n ");
            printf("-s --start                     to auto start game\n");
            printf("-h --help                      to show this help\n");
        }
    }
    scanSkins();
    if (st) {
        hide();
        start();
        return;
    }

    QObject::connect(ui->commandLinkButton, SIGNAL(clicked()), this, SLOT(start()));
    QObject::connect(ui->comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(setPix(QString)));
}

void startDialog::start() {
    w = new MainWindow(app, QHostAddress(ui->lineEdit_2->text()), ui->spinBox->value(), ui->lineEdit->text().toAscii(), skinPath + ui->comboBox->currentText(), this);
    connectWindow = new Connection(QString("Connecting to: ") + ui->lineEdit_2->text() + ":" + ui->spinBox->text());
    connectWindow->show();

    QObject::connect(w, SIGNAL(fail()), this, SLOT(show()));
    QObject::connect(w, SIGNAL(successConnection()), connectWindow, SLOT(hide()));
    QObject::connect(w, SIGNAL(fail()), connectWindow, SLOT(hide()));
    QObject::connect(w, SIGNAL(fail()), w, SLOT(deleteLater()));
    QObject::connect(w, SIGNAL(successConnection()), w->widget, SLOT(show()));

    if (ui->fullScreen->checkState() == Qt::Checked)
        QObject::connect(w, SIGNAL(successConnection()), w->widget, SLOT(showFullScreen()));

//    command->show();
    this->hide();

    QEventLoop *loop = new QEventLoop;
    loop->connect(w, SIGNAL(successConnection()), loop, SLOT(quit()));
    loop->connect(w, SIGNAL(fail()), loop, SLOT(quit()));
    loop->exec();
}

void startDialog::scanSkins() {
    skinPath = "../skins/";
    QFileInfoList list = QDir(skinPath).entryInfoList();
    if (list.size() <= 2) {
        skinPath = "/usr/share/labyrus/skins/";
        list = QDir(skinPath).entryInfoList();
    }

    for (int i = 0; i < list.size(); i++)
        if ((list.at(i).fileName() != ".")  && (list.at(i).fileName() != ".."))
          ui->comboBox->addItem(list.at(i).fileName());

    setPix(ui->comboBox->currentText());
}

void startDialog::setPix(QString s) {
    pix = new QPixmap(skinPath + s + "/defaultWall.png");
    repaint();
}

void startDialog::paintEvent(QPaintEvent *event) {
    QPainter p(this);

    p.drawPixmap(0, 130, this->width(), 300, *pix);

    p.end();
    event->accept();
}
