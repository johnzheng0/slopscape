#include "scanmanager.h"
#include "ui_scanmanager.h"
#include <unistd.h>

ScanManager::ScanManager(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ScanManager)
    , settings(QCoreApplication::applicationName(), QCoreApplication::applicationName())
    , scanCount(0)
{
    ui->setupUi(this);
    connect(this, &ScanManager::scanDone, this, &ScanManager::updateStatus);
    hide();
}

ScanManager::~ScanManager()
{
    delete ui;
}

void ScanManager::execScan()
{
    ScanDialog dialog = ScanDialog();
    if (dialog.exec()) {
        scanCount++;
        show();
        QFuture<void> scan = QtConcurrent::run(&ScanManager::scan, this, dialog.getDirectory());
    }
}

void ScanManager::scan(const QString location)
{
    QFileInfo locationInfo(location);
    if (!locationInfo.exists()) return;
    if (locationInfo.isDir()) {
        // filepath lists initialization
        QStringList locations;

        // puts filePaths into lists
        QDirIterator it(location, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString location = it.next();
            QFileInfo fileInfo(location);
            QString extension = fileInfo.suffix().toLower();

            // Check file extension
            if (settings.value("File Extensions").toString().split(" ").contains(extension)) {
                locations.push_back(location);
            }
        }

        // insert files
        db.insertFiles(locations);
    } else if (locationInfo.isFile()) {
        QString extension = locationInfo.suffix().toLower();

        // Check file extension
        if (settings.value("File Extensions").toString().split(" ").contains(extension)) {
            // insert file
            db.insertFiles({location});
        }
    }

    emit scanDone();
}

void ScanManager::updateStatus()
{
    scanCount--;
    if (scanCount == 0) hide();
}
