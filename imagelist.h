#ifndef IMAGELIST_H
#define IMAGELIST_H

#include <QWidget>
#include <QCoreApplication>
#include "configs.h"
#include <QSettings>
#include "database.h"
#include "imagecard.h"
#include <QResizeEvent>
#include <QApplication>
#include "dialogedit.h"
#include "dialogconfirm.h"
#include <QSpinBox>

namespace Ui {
class ImageList;
}

class ImageList : public QWidget
{
    Q_OBJECT

public:
    explicit ImageList(QWidget *parent = nullptr);
    ~ImageList();

    void load(QVector<FileRecord> files);
    void clear();
protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    Ui::ImageList *ui;
    QSettings settings;
    Database *db;
    QVector<FileRecord> files;
    QVector<ImageCard*> gridItems;
    QFuture<void> tagEditThread;

    // void display();
    void display(size_t page=1);
    void resizeGrid();

    void execEdit();
    void execDelete();
    void execShowUntagged();
    void execShowTagged();
    void execShowAll();
    void addTagsToFiles(QVector<ImageCard *> cards, DialogEdit::Modifier modifier, QVector<TagRecord> tags);

    void pageNext();
    void pagePrev();

    void selectCard(ImageCard* card);
    ImageCard *selectionAnchor;
    ImageCard *selectionTail;

    void sliderFix(int value);
signals:
    void updated(QWidget* view);
    void cardClicked(ImageCard *card);
    void cardDoubleClicked(ImageCard *card);
    void signalDeleteCard(ImageCard *card);
    void cardAdded(ImageCard *card);
};

#endif // IMAGELIST_H
