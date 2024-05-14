#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "colors.h"
#include "configs.h"
#include <QSettings>
#include "settings.h"
#include "database.h"
#include "imagelist.h"
#include "imagecard.h"
#include "viewermain.h"
#include "scanmanager.h"
#include "dialogedit.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void add_image(FileRecord file);
    void view_card(ImageCard *card);
    QSettings settings;
    Database db;

private:
    Ui::MainWindow *ui;
    void execScan();
    void execFilter(QVector<TagRecord> tags);
    void execTagClicked(TagChip *chip);
    void execAutoTag(FileRecord file);
    void execAutoTagTrain();
    void execSettings();
    void onListCardAdded(ImageCard* card);

signals:
    void windowResized(int width);
};
#endif // MAINWINDOW_H
