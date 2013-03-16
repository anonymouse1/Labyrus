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
        else if ((cur == "-s") || (cur == "--start"))
            st = true;
        else if ((cur == "-h") || (cur == "--help")) {
            printf("Welcome to labyrus-client documentation\n");
            printf("-i --ip int.int.int.int        to set ip address\n");
            printf("-p --port int                  to set port number\n");
            printf("-n --name name                 to set name of player\n");
            printf("-s --start                     to auto start game\n");
            printf("-h --help                      to show this help\n");
            exit(0);
        }
    }
    scanSkins();

    ui->comboBox->setCurrentIndex(ui->comboBox->findText("default"));

    if (st) {
        QTimer::singleShot(100, this, SLOT(start()));
    } else {
        QObject::connect(ui->commandLinkButton, SIGNAL(clicked()), this, SLOT(start()));
        QObject::connect(ui->comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(setPix(QString)));
        loadSettings();
    }
    ui->commandLinkButton->setFocus();
}

void startDialog::start() {
    w = new MainWindow(app, QHostAddress(ui->lineEdit_2->text()), ui->spinBox->value(), ui->lineEdit->text().toLocal8Bit(), skinPath + ui->comboBox->currentText(), mouseSensitivity, this);
    connectWindow = new Connection(tr("Connecting to: ") + ui->lineEdit_2->text() + ":" + ui->spinBox->text());
    this->hide();
    connectWindow->show();

    QObject::connect(w, SIGNAL(fail()), this, SLOT(show()));
    QObject::connect(w, SIGNAL(successConnection()), connectWindow, SLOT(hide()));
    QObject::connect(w, SIGNAL(fail()), connectWindow, SLOT(hide()));
    QObject::connect(w, SIGNAL(fail()), w, SLOT(deleteLater()));
    QObject::connect(w, SIGNAL(successConnection()), w->widget, SLOT(show()));

    if (ui->fullScreen->checkState() == Qt::Checked)
        QObject::connect(w, SIGNAL(successConnection()), w->widget, SLOT(showFullScreen()));

    QEventLoop *loop = new QEventLoop;
    loop->connect(w, SIGNAL(successConnection()), loop, SLOT(quit()));
    loop->connect(w, SIGNAL(fail()), loop, SLOT(quit()));
    loop->exec();
}

void startDialog::scanSkins() {
    QFileInfoList list = QDir(skinPath).entryInfoList();

    for (int i = 0; i < list.size(); i++)
        if ((list.at(i).fileName() != ".")  && (list.at(i).fileName() != ".."))
          ui->comboBox->addItem(list.at(i).fileName());

    setPix(ui->comboBox->currentText());
}

void startDialog::setPix(QString s) {
    pix = new QPixmap(skinPath + s + "/defaultWall.jpg");
    repaint();
}

void startDialog::paintEvent(QPaintEvent *event) {
    QPainter p(this);

    p.drawPixmap(0, 130, this->width(), 300, *pix);

    p.end();
    event->accept();
}

void startDialog::saveSettings() {
    QSettings s(settingsFile, QSettings::IniFormat);
    s.setValue("dialogGeometry", QVariant(saveGeometry()));
    s.setValue("ip", QVariant(ui->lineEdit_2->text()));
    s.setValue("name", QVariant(ui->lineEdit->text()));
    s.setValue("port", QVariant(ui->spinBox->value()));
    s.setValue("fullscreen", QVariant(ui->fullScreen->checkState() == Qt::Checked));
    s.setValue("skin", QVariant(ui->comboBox->currentText()));
}

void startDialog::loadSettings() {
    QSettings s(settingsFile, QSettings::IniFormat);
    restoreGeometry(s.value("dialogGeometry").toByteArray());
    ui->lineEdit->setText(s.value("name", QVariant("vlad")).toString());
    ui->lineEdit_2->setText(s.value("ip", QVariant("127.0.0.1")).toString());
    ui->spinBox->setValue(s.value("port", QVariant(7777)).toInt());
    ui->fullScreen->setChecked(s.value("fullscreen", QVariant(true)).toBool());
    ui->comboBox->setCurrentIndex(ui->comboBox->findText(s.value("skin", QVariant("default")).toString()));
    mouseSensitivity = s.value("mouseSensitivity", QVariant(0.3)).toDouble();
    setPix(ui->comboBox->currentText());
}

startDialog::~startDialog() {
    saveSettings();
}
