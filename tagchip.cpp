#include "tagchip.h"
#include "ui_tagchip.h"

TagChip::TagChip(TagRecord tagIn, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TagChip)
    , tag(tagIn)
{
    ui->setupUi(this);
    ui->label->setText(tag.getName());
}

TagChip::~TagChip()
{
    delete ui;
}

void TagChip::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked(this);
    }
}
