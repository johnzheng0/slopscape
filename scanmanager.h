#ifndef SCANMANAGER_H
#define SCANMANAGER_H

#include <QWidget>
#include <QList>
#include "scandialog.h"
#include "database.h"
#include <QtConcurrent/QtConcurrent>
#include <QCoreApplication>
#include <QLabel>
#include <QSettings>

namespace Ui {
class ScanManager;
}

class ScanManager : public QWidget
{
    Q_OBJECT

public:
    explicit ScanManager(QWidget *parent = nullptr);
    ~ScanManager();

    void execScan();

private:
    Ui::ScanManager *ui;
    QSettings settings;
    Database db;
    void scan(const QString directory);
    size_t scanCount;
    void updateStatus();

signals:
    void scanDone();
    void scanUpdateProgressBar(int value, int max);

};

#endif // SCANMANAGER_H
