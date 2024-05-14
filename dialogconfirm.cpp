#include "dialogconfirm.h"
#include "ui_dialogconfirm.h"

DialogConfirm::DialogConfirm(QString message, QString acceptLabel, QString rejectLabel, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogConfirm)
{
    ui->setupUi(this);

    ui->label->setText(message);
    ui->buttonAccept->setText(acceptLabel);
    ui->buttonReject->setText(rejectLabel);

    connect(ui->buttonAccept, &QPushButton::clicked, this, &DialogConfirm::accept);
    connect(ui->buttonReject, &QPushButton::clicked, this, &DialogConfirm::reject);
}

DialogConfirm::~DialogConfirm()
{
    delete ui;
}
