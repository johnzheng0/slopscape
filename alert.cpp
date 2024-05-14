#include "alert.h"
#include "ui_alert.h"

Alert::Alert(QString message, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Alert)
{
    ui->setupUi(this);
    ui->label_message->setText(message);
}

Alert::~Alert()
{
    delete ui;
}
