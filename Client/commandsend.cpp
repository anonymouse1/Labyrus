#include "commandsend.h"

CommandSend::CommandSend(QTcpSocket *s, QWidget *parent) :
    QWidget(parent)
{
    ui = new Ui::commands();
    ui->setupUi(this);

    socket = s;
    wasPrinted("b - to build wall\n");
    wasPrinted("space - to kill players\n");
    wasPrinted("e - to erase wall\n");

    QObject::connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(print()));
}

void CommandSend::go(QString s, bool flush, bool b) {
    if (s == "bot") {
        emit startBot();
        return;
    }

    if (b)
        s += "\n";
    socket->write(s.toAscii());
    if (flush)
        socket->flush(); // it was a many strange bugs, was not it?

    if (b)
        wasPrinted(s);
    else
        wasPrinted(s + "\n");
}

void CommandSend::print() {
    go(ui->lineEdit->text(), true);
}

void CommandSend::wasPrinted(QString s) {
    ui->textBrowser->insertPlainText(s);
}
