#ifndef SCANDIALOG_H
#define SCANDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QCoreApplication>
#include <QFileDialog>

namespace Ui {
class ScanDialog;
}

class ScanDialog: public QDialog
{
    Q_OBJECT

public:
    explicit ScanDialog(QWidget *parent = nullptr);
    ~ScanDialog();

    QString getDirectory();

private:
    Ui::ScanDialog*ui;
    QSettings settings;
    void onButtonDirectoryLocation();
    void onButtonFileLocation();
    void onAccept();

signals:
    void scanDone();
};

#endif // SCANDIALOG_H
