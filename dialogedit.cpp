#include "dialogedit.h"
#include "ui_dialogedit.h"

void DialogEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        return;
    QDialog::keyPressEvent(event);
}

DialogEdit::DialogEdit(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogEdit)
{
    ui->setupUi(this);
    connect(ui->buttonAccept, &QPushButton::clicked, this, &DialogEdit::accept);
    connect(ui->buttonCancel, &QPushButton::clicked, this, &DialogEdit::reject);
    connect(ui->inputTags, &InputTags::returnPressed, this, &DialogEdit::accept);
}

DialogEdit::~DialogEdit()
{
    delete ui;
}

QVector<TagRecord> DialogEdit::getTags()
{
    return ui->inputTags->getTags();
}

void DialogEdit::setTags(QVector<TagRecord> tags)
{
    ui->inputTags->setTags(tags);
}

void DialogEdit::setModifier(Modifier modifier)
{
    switch(modifier) {
    case Add:
        ui->radioAdd->setChecked(true);
        break;
    case Remove:
        ui->radioRemove->setChecked(true);
        break;
    case Overwrite:
        ui->radioOverwrite->setChecked(true);
        break;
    default:
        break;
    }
}

DialogEdit::Modifier DialogEdit::getModifier()
{
    if (ui->radioAdd->isChecked()) {
        return DialogEdit::Add;
    } else if (ui->radioRemove->isChecked()) {
        return DialogEdit::Remove;
    } else if (ui->radioOverwrite->isChecked()) {
        return DialogEdit::Overwrite;
    } else {
        return DialogEdit::Add;
    }
}
