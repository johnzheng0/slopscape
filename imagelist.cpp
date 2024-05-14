#include "imagelist.h"
#include "ui_imagelist.h"

ImageList::ImageList(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImageList)
    , settings(QCoreApplication::applicationName(), QCoreApplication::applicationName())
    , db(new Database)
{
    ui->setupUi(this);

    connect(ui->buttonLeft, &QToolButton::clicked, this, &ImageList::pagePrev);
    connect(ui->buttonRight, &QToolButton::clicked, this, &ImageList::pageNext);
    connect(this, &ImageList::cardClicked, this, &ImageList::selectCard);
    connect(ui->buttonEdit, &QToolButton::clicked, this, &ImageList::execEdit);
    connect(ui->buttonDelete, &QToolButton::clicked, this, &ImageList::execDelete);
    connect(ui->buttonShowUntagged, &QToolButton::clicked, this, &ImageList::execShowUntagged);
    connect(ui->buttonShowAll, &QToolButton::clicked, this, &ImageList::execShowAll);
    connect(ui->inputPage, &QSpinBox::valueChanged, this, &ImageList::display);
}

ImageList::~ImageList()
{
    clear();
    delete db;
    delete ui;
}

void ImageList::load(QVector<FileRecord> filesIn)
{
    // update member variables
    files = filesIn;

    // delete the non existent files
    QVector<FileRecord> filesToDelete;
    files.removeIf([&filesToDelete](auto& file) {
        bool notExist = !QFileInfo(file.getLocation()).exists();
        if (notExist)filesToDelete.push_back(file);
        return notExist;
    });
    db->deleteFiles(filesToDelete);

    // shuffle if shuffle is toggled on
    if (settings.value("Shuffle").toBool())
        for (auto& file : files) std::swap(file, files[random() % files.size()]);

    // display first page
    this->display(1);
}

void ImageList::clear()
{
    // reset member variables
    selectionAnchor = nullptr;
    selectionTail = nullptr;
    gridItems.clear();

    // clears grid layout
    while (auto child = ui->list->takeAt(0)) {
        delete child->widget();
    }
}

void ImageList::resizeEvent(QResizeEvent*)
{
    resizeGrid();
}

void ImageList::display(size_t page)
{
    // clears and checks if any files to display
    clear();
    if (files.isEmpty()) return;

    // calculates column count
    size_t maxWidth = settings.value("Card Size").toInt();
    int columnCount = ui->scrollArea->width() / (maxWidth+8);
    if (columnCount <= 0) columnCount = 1;

    // gets iterators based on page and sets paging button states
    size_t limit = settings.value("Card Limit").toUInt();
    QVector<FileRecord>::iterator start, end;
    if (limit == 0) {
        // display all files and disables paging buttons
        start = files.begin();
        end = files.end();
        ui->buttonLeft->setEnabled(false);
        ui->buttonRight->setEnabled(false);
    } else {
        // limits page to maximum
        while (files.begin() + (page-1) * limit >= files.end()) {
            page --;
        }
        // sets iterators to show only those on the page and sets button states
        start = files.begin() + (page-1) * limit;
        if (files.begin() + page * limit >= files.end()) {
            end = files.end();
            ui->buttonRight->setEnabled(false);
        } else {
            end = files.begin() + page * limit;
            ui->buttonRight->setEnabled(true);
        }
        if (page <= 1) ui->buttonLeft->setEnabled(false);
        else ui->buttonLeft->setEnabled(true);
    }

    // update page spinbox value
    ui->inputPage->blockSignals(true);
    ui->inputPage->setValue(page);
    ui->inputPage->blockSignals(false);

    // creates cards and displays them
    int index = 0;
    for (auto it=start; it!=end; it++) {
        ImageCard *card = new ImageCard(db, *it, maxWidth);
        ui->list->addWidget(card, index/columnCount, index % columnCount);
        connect(card, &ImageCard::clicked, this, &ImageList::cardClicked);
        connect(card, &ImageCard::doubleClicked, this, &ImageList::cardDoubleClicked);
        gridItems.push_back(card);
        index++;
        emit cardAdded(card);
    }

    emit updated(this);
}

void ImageList::resizeGrid()
{
    // calculates column count and ignores resizing if unchanged prior
    int maxWidth = settings.value("Card Size").toInt();
    int columnCount = ui->scrollArea->width() / (maxWidth+8);
    if (columnCount == ui->list->columnCount()) return;
    if (columnCount <= 0) return;

    // places cards in grid
    size_t itemCount = ui->list->count();
    for (size_t i=0; i<itemCount; i++) {
        ui->list->addWidget(gridItems[i], i/columnCount, i % columnCount);
    }
}

void ImageList::pageNext()
{
    // display next page
    this->display(ui->inputPage->value()+1);
}

void ImageList::pagePrev()
{
    // display previous page
    this->display(ui->inputPage->value()-1);
}

void ImageList::selectCard(ImageCard *card)
{
    // control mask case
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        card->select(!(card->selected));
        selectionAnchor = card;
        selectionTail = nullptr;
        return;
    }

    // shift mask case
    if (selectionAnchor != nullptr && QApplication::keyboardModifiers() & Qt::ShiftModifier) {
        bool selectionMode = selectionAnchor->selected;

        // clear selections from tail to anchor
        if (selectionTail != nullptr) {
            size_t start = ui->list->indexOf(selectionAnchor);
            size_t end = ui->list->indexOf(selectionTail);
            if (start > end) std::swap(start, end);
            for (size_t i=start; i<=end; i++) {
                gridItems[i]->select(false);
            }
        }

        // apply selection from the anchor to selected card
        size_t start = ui->list->indexOf(selectionAnchor);
        size_t end = ui->list->indexOf(card);
        if (start > end) std::swap(start, end);
        for (size_t i=start; i<=end; i++) {
            gridItems[i]->select(selectionMode);
        }

        selectionTail = card;
        return;
    }

    // no mask case
    for (auto& item : gridItems) {
        if (item->selected) item->select(false);
    }
    card->select(true);
    selectionAnchor = card;
    selectionTail = nullptr;
}


void ImageList::execEdit()
{
    // gets selected cards
    QVector<ImageCard*> cards;
    for (auto& item : gridItems) {
        if (item->selected) cards.push_back(item);
    }
    // executes edit tags dialog and edit tags
    DialogEdit dialog{};
    if (dialog.exec()) {
        QVector<TagRecord> tags = dialog.getTags();
        DialogEdit::Modifier modifier = dialog.getModifier();
        tagEditThread.waitForFinished();
        tagEditThread = QtConcurrent::run(&ImageList::addTagsToFiles, this, cards, modifier, tags);
    }
}

void ImageList::addTagsToFiles(QVector<ImageCard*> cards, DialogEdit::Modifier modifier, QVector<TagRecord> tags)
{
    // get list of file records from cards
    QVector<FileRecord> selectedFiles;
    for (auto& card : cards) selectedFiles.push_back(card->file);
    // makes changes to database depending on modifier
    switch(modifier) {
    case DialogEdit::Add:
        db->attachTagsToFiles(selectedFiles, tags);
        break;
    case DialogEdit::Remove:
        db->removeTagsFromFiles(selectedFiles, tags);
        break;
    case DialogEdit::Overwrite:
        db->removeAllTagsFromFiles(selectedFiles);
        db->attachTagsToFiles(selectedFiles, tags);
        break;
    }
    // updates tags of selected cards
    for (auto& card : cards) card->updateTags();
}

void ImageList::execDelete()
{
    // get selected cards
    QVector<ImageCard*> cardsToDelete;
    for (auto& item : gridItems) {
        if (item->selected) cardsToDelete.push_back(item);
    }
    if (cardsToDelete.isEmpty()) return;

    // executes comfirmation dialog
    DialogConfirm dialog("Are you sure you want to delete "+QString::number(cardsToDelete.size())+" items?", "Delete");
    if (dialog.exec()) {
        // does the deleting
        QVector<FileRecord> filesToDelete;
        for (auto& item : cardsToDelete) {
            filesToDelete.push_back(item->file);
            files.removeOne(item->file);
            gridItems.removeOne(item);
            ui->list->removeWidget(item);
            delete item;
        }
        db->deleteFiles(filesToDelete);

        // forces resizing
        int maxWidth = settings.value("Card Size").toInt();
        int columnCount = ui->scrollArea->width() / (maxWidth+8);
        if (columnCount <= 0) return;
        size_t itemCount = ui->list->count();
        for (size_t i=0; i<itemCount; i++) {
            ui->list->addWidget(gridItems[i], i/columnCount, i % columnCount);
        }
    }
}

void ImageList::execShowUntagged()
{
    load(db->getFilesWithoutTags());
}

void ImageList::execShowAll()
{
    load(db->getFilesWithTags());
}
