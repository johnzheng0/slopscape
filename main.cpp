#include "mainwindow.h"

#include <QApplication>
#include <QResource>
#include "database.h"
#include "opencv2/opencv.hpp"

#define SETTING_DEFAULT(key, value) \
if (!settings.contains(key)) {settings.setValue(key, value);}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QFile file(":/theme/theme/theme.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(file.readAll());
        a.setStyleSheet(styleSheet);
        file.close();
    } else {
        return 0;
    }

    // init settings
    QSettings settings(QCoreApplication::applicationName(), QCoreApplication::applicationName());
    SETTING_DEFAULT("Card Size", 200);
    SETTING_DEFAULT("Card Limit", 60);
    SETTING_DEFAULT("Scan Location", "");
    SETTING_DEFAULT("File Extensions", "jpg jpeg png bmp gif mp4 webm");
    SETTING_DEFAULT("File Manager Bin", "thunar");

    return a.exec();
}
