#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , settings(QCoreApplication::applicationName(), QCoreApplication::applicationName())
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db.initTables();
    ui->buttonToggleShuffle->setChecked(settings.value("Shuffle").toBool());

    connect(ui->inputFilter, &InputLineTags::returnPressed, this, &MainWindow::execFilter);
    connect(ui->button_scan, &QToolButton::clicked, this, &MainWindow::execScan);
    connect(ui->buttonToggleShuffle, &QToolButton::clicked, [&](bool shuffle) {settings.setValue("Shuffle", shuffle);});

    connect(ui->tabList, &ImageList::updated, ui->tabWidget, &QTabWidget::setCurrentWidget);
    connect(ui->tabView, &ViewerMain::updated, ui->tabWidget, &QTabWidget::setCurrentWidget);
    connect(ui->tabList, &ImageList::cardDoubleClicked, this, &MainWindow::view_card);
    connect(ui->tabView, &ViewerMain::tagClicked, this, &MainWindow::execTagClicked);
    connect(ui->tabList, &ImageList::cardAdded, this, &MainWindow::onListCardAdded);

    connect(ui->tabView, &ViewerMain::autoTagClicked, this, &MainWindow::execAutoTag);
    connect(ui->buttonAutoTagger, &QAbstractButton::clicked, this, &MainWindow::execAutoTagTrain);

    connect(ui->buttonSettings, &QAbstractButton::clicked, this, &MainWindow::execSettings);
}

MainWindow::~MainWindow()
{
    db.deleteUnusedJunctions();
    db.deleteUnusedTags();
    delete ui;
}

void MainWindow::view_card(ImageCard *card)
{
    connect(card, &ImageCard::tagsUpdated, ui->tabView, &ViewerMain::updateTags);
    connect(ui->tabView, &ViewerMain::tagsEdited, card, &ImageCard::updateTags);
    ui->tabView->load(card->file);
}

void MainWindow::execScan()
{
    ui->scanManager->execScan();
}

void MainWindow::execFilter(QVector<TagRecord> tags)
{
    QVector<FileRecord> files = db.getFilesFromTags(tags);
    ui->tabList->load(files);
}

void MainWindow::execTagClicked(TagChip *chip)
{
    ui->inputFilter->setTags({chip->getTag()});
    ui->inputFilter->send();
}

void MainWindow::execSettings()
{
    if (Settings().exec()) {

    }
}

void MainWindow::execAutoTag(FileRecord file)
{
    QVector<TagRecord> predictedTags = ui->autotagger->predict(file);
    DialogEdit dialog;
    dialog.setTags(predictedTags);
    if (dialog.exec()) {
        QThreadPool::globalInstance()->start([&]() {
            ui->tabView->editTags(dialog.getTags(), dialog.getModifier());
        });
    }
}

void MainWindow::execAutoTagTrain()
{
    DialogConfirm dialog("Train AutoTagger?");
    if (dialog.exec()) {
        ui->autotagger->train();
    }
}

void MainWindow::onListCardAdded(ImageCard *card)
{
    if (ui->tabView->getFile() == card->file) {
        connect(card, &ImageCard::tagsUpdated, ui->tabView, &ViewerMain::updateTags);
        connect(ui->tabView, &ViewerMain::tagsEdited, card, &ImageCard::updateTags);
    }
}


