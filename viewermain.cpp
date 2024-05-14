#include "viewermain.h"
#include "ui_viewermain.h"
#include <iostream>

ViewerMain::ViewerMain(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ViewerMain)
    , settings(QCoreApplication::applicationName(), QCoreApplication::applicationName())
    , file(FileRecord(""))
{
    ui->setupUi(this);
    // connect(ui->buttonShowInFileManager, &QToolButton::clicked, this, &ViewerMain::openInFileManager);
    connect(ui->buttonFileManager, &QPushButton::clicked, this, &ViewerMain::openInFileManager);
    connect(ui->buttonEdit, &QPushButton::clicked, this, &ViewerMain::execEditTags);
    connect(this, &ViewerMain::tagsEdited, this, &ViewerMain::updateTags);
    connect(ui->buttonAutoTag, &QPushButton::clicked, [&]() {emit autoTagClicked(file);});
}

ViewerMain::~ViewerMain()
{
    delete ui;
}

void ViewerMain::load(FileRecord file)
{
    // update member variables
    this->file = file;

    // update display elements
    QThreadPool::globalInstance()->start([=]() {loadFile(file);});
    ui->buttonFileManager->setToolTip(file.getLocation());
    updateTags();

    emit updated(this);
}

void ViewerMain::clear()
{
    // clears both the image and video viewers
    ui->imageView->clear();
    ui->videoView->clear();
}

void ViewerMain::loadFile(FileRecord file)
{
    clear();

    // determine the type of the file
    QFileInfo fileInfo(file.getLocation());
    QString extension = fileInfo.suffix().toLower();
    if (extension == "mp4" || extension == "webm" || extension == "gif") {
        // load file contents as a video
        ui->stackWidget->setCurrentWidget(ui->videoView);
        ui->videoView->load(file);
    } else {
        // load file contents as an image
        ui->stackWidget->setCurrentWidget(ui->imageView);
        ui->imageView->load(file);
    }
}

void ViewerMain::execEditTags()
{
    // execute tag edit dialog
    DialogEdit dialog;
    dialog.setTags(tags);
    dialog.setModifier(DialogEdit::Overwrite);
    if (dialog.exec()) {
        QThreadPool::globalInstance()->start([&]() {
            editTags(dialog.getTags(), dialog.getModifier());
        });
    }
}

void ViewerMain::updateTags()
{
    // clear tag layout
    for (auto child : ui->tagLayout->children()) {
        disconnect(child);
        child->deleteLater();
        // delete child;
    }

    // get tags and create the chips for them
    tags = db.getFileTags(file);
    QVector<TagChip*> chips;
    for (auto& tag : tags) {
        TagChip *chip = new TagChip(tag, ui->tagLayout);
        chips.push_front(chip);
        connect(chip, &TagChip::clicked, this, &ViewerMain::tagClicked);
    }

    // logic that places the chips on the tag layout
    int x = 2;
    int y = 2;
    int maxRowY = 0;
    for (auto& chip : chips) {
        chip->show();
        chip->setGeometry(chip->layout()->itemAt(0)->geometry());
        if (chip->height() > maxRowY) {
            maxRowY = chip->height();
        }
        if (x+chip->width() > ui->tagLayout->width()) {
            x = 2;
            y += maxRowY + 2;
            maxRowY = 0;
        }
        chip->setGeometry(x, y, chip->width(), chip->height());
        x += chip->width() + 2;
    }
}

void ViewerMain::openInFileManager()
{
// open file location in the file manager
#ifdef Q_OS_WIN
    // for windows
    QDesktopServices::openUrl(QUrl::fromLocalFile(file.getLocation()));
#else
    // for linux/mac
    QProcess::startDetached(settings.value("File Manager Bin").toString(), QStringList() << file.getLocation());
#endif
}

void ViewerMain::editTags(QVector<TagRecord> dialogTags, DialogEdit::Modifier modifier)
{
    // get data needed for database queries
    QVector<TagRecord> currentTags = tags;

    // makes changes to database depending on modifier
    switch(modifier) {
    case DialogEdit::Add:
        db.attachTagsToFiles({file}, dialogTags);
        break;
    case DialogEdit::Remove:
        db.removeTagsFromFiles({file}, dialogTags);
        break;
    case DialogEdit::Overwrite:
        // remove unspecifed tags and add newly specified tags
        QVector<TagRecord> tagsRemove;
        QVector<TagRecord> tagsAdd;
        for (auto& tag : currentTags) {
            if (!dialogTags.contains(tag)) tagsRemove.push_back(tag);
        }
        for (auto& tag : dialogTags) {
            if (!currentTags.contains(tag)) tagsAdd.push_back(tag);
        }
        db.removeTagsFromFiles({file}, tagsRemove);
        db.attachTagsToFiles({file}, tagsAdd);
        break;
    }
    // update tags
    emit tagsEdited();
}
