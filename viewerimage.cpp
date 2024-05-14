#include "viewerimage.h"
#include "ui_viewerimage.h"

ViewerImage::ViewerImage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ViewerImage)
    , image(QPixmap())
{
    ui->setupUi(this);
}

ViewerImage::~ViewerImage()
{
    delete ui;
}

void ViewerImage::load(FileRecord file)
{
    QPixmap image = QPixmap(file.getLocation());
    this->image = image;

    ui->image->setPixmap(image);
    ui->imageContainer->setMaximumSize(image.size().scaled(this->size(), Qt::KeepAspectRatio));
}

void ViewerImage::clear()
{
    ui->image->clear();
}

void ViewerImage::resizeEvent(QResizeEvent*)
{
    ui->imageContainer->setMaximumSize(image.size().scaled(this->size(), Qt::KeepAspectRatio));
}
