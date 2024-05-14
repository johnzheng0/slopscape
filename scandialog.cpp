#include "scandialog.h"
#include "ui_scandialog.h"

ScanDialog::ScanDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ScanDialog)
    , settings(QCoreApplication::applicationName(), QCoreApplication::applicationName())
{
    ui->setupUi(this);

    connect(ui->buttonCancel, &QPushButton::clicked, this, &ScanDialog::reject);
    connect(ui->buttonScan, &QPushButton::clicked, this, &ScanDialog::onAccept);
    connect(ui->buttonDirectory, &QToolButton::clicked, this, &ScanDialog::onButtonDirectoryLocation);
    ui->inputLocation->setText(settings.value("Scan Location").toString());
}

ScanDialog::~ScanDialog()
{
    delete ui;
}

QString ScanDialog::getDirectory()
{
    return ui->inputLocation->text();
}

void ScanDialog::onButtonDirectoryLocation()
{
    ui->inputLocation->setText(QFileDialog::getExistingDirectory());
}

void ScanDialog::onButtonFileLocation()
{
    ui->inputLocation->setText(QFileDialog::getOpenFileName());
}

void ScanDialog::onAccept()
{
    settings.setValue("Scan Location", ui->inputLocation->text());
    this->accept();
}
