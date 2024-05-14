#ifndef IMAGECARD_H
#define IMAGECARD_H

#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QFileInfo>
#include "database.h"
#include <QMouseEvent>
#include <QtConcurrent/QtConcurrent>
#include <QFileInfo>
#include <opencv2/opencv.hpp>

namespace Ui {
class ImageCard;
}

class ImageCard : public QWidget
{
    Q_OBJECT

public:
    explicit ImageCard(Database *dbIn, FileRecord fileIn, size_t maxWidth, QWidget *parent = nullptr);
    ~ImageCard();

    FileRecord file;
    bool selected;
    void select(bool selected);
    void resizeImage(size_t maxWidth);

    void updateTags();
protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::ImageCard *ui;
    QFuture<void> init;
    QPixmap image;
    void loadImage(size_t maxWidth);
    Database *db;

signals:
    void clicked(ImageCard *card);
    void doubleClicked(ImageCard *card);
    void tagsUpdated();
};

#endif // IMAGECARD_H
