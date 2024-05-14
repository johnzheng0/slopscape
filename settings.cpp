#include "settings.h"
#include "ui_settings.h"
#include <QProcess>

Settings::Settings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Settings)
    , settings(QCoreApplication::applicationName(), QCoreApplication::applicationName())
{
    ui->setupUi(this);

    ui->inputExtensions->setText(settings.value("File Extensions").toString());
    ui->inputItemsPerPage->setValue(settings.value("Card Limit").toInt());
    ui->inputFileManager->setText(settings.value("File Manager Bin").toString());

    connect(ui->buttonClose, &QAbstractButton::clicked, [&]() {
        settings.setValue("File Extensions", ui->inputExtensions->text());
        settings.setValue("Card Limit", ui->inputItemsPerPage->value());
        settings.setValue("File Manager Bin", ui->inputFileManager->text());
        accept();
    });

    connect (ui->buttonDeleteData, &QAbstractButton::clicked, [&]() {
        QString dataDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
        QFile(dataDir + "/app.db").remove();
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    });
    connect(ui->buttonClearAutoTag, &QAbstractButton::clicked, [&]() {
        QString dataDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
        QFile(dataDir + "/model.xml").remove();
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    });
}

Settings::~Settings()
{
    delete ui;
}
