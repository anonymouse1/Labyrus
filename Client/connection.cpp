#include "connection.h"
#include "ui_connection.h"

Connection::Connection(QString s, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Connection)
{
    ui->setupUi(this);
    ui->label->setText(s);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(0);
}

Connection::~Connection()
{
    delete ui;
}
