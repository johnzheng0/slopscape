#include "inputlinetags.h"
#include "ui_inputlinetags.h"
#include <QDebug>


InputLineTags::InputLineTags(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InputLineTags)
    , model(new QStringListModel())
    , emptyModel(new QStringListModel())
    , completer(new QCompleter(model))
    , db(Database())
{
    ui->setupUi(this);

    this->setFocusProxy(ui->inputName);

    completer->setCompletionMode(QCompleter::InlineCompletion);
    ui->inputName->setCompleter(completer);

    connect(ui->inputName, &LineEdit::focusedIn, this, &InputLineTags::enterEdit);
    connect(ui->inputName, &LineEdit::returnPressed, this, &InputLineTags::handleReturn);
    connect(ui->inputName, &LineEdit::emptyBackspace, this, &InputLineTags::handleBackspace);
    connect(ui->inputName, &LineEdit::textChanged, this, &InputLineTags::handleEdit);
}

InputLineTags::~InputLineTags()
{
    delete model;
    delete completer;
    delete ui;
}

void InputLineTags::handleEdit(const QString text)
{
    if (text.size() > 2) completer->setModel(emptyModel);
    else completer->setModel(model);

    QStringList tagNames = text.split(" ");
    if (!tagNames.last().isEmpty()) {
        return;
    }

    tagNames.removeAll("");

    for (auto& tagName : tagNames) {
        TagChip *chip = new TagChip(db.getTag(tagName));
        ui->tagStack->addWidget(chip);
        connect(chip, &TagChip::clicked, chip, &TagChip::deleteLater);
    }

    ui->inputName->clear();
}

void InputLineTags::handleReturn()
{
    if (ui->inputName->hasSelectedText()) {
        QKeyEvent complete(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
        QApplication::sendEvent(ui->inputName, &complete);
        return;
    }
    handleEdit(ui->inputName->text()+" ");

    QVector<TagRecord> tags;
    int tagCount = ui->tagStack->count();
    for (int i=0; i<tagCount; i++) {
        if (TagChip *tagChip = dynamic_cast<TagChip*>(ui->tagStack->itemAt(i)->widget())) {
            tags.push_back(tagChip->getTag());
        }
    }

    emit returnPressed(tags);
}

void InputLineTags::handleBackspace()
{
    if (!ui->tagStack->itemAt(0)) return;

    QLayoutItem *item = ui->tagStack->itemAt(ui->tagStack->count()-1);
    if (TagChip *tagChip = dynamic_cast<TagChip*>(item->widget())) {
        ui->inputName->setText(tagChip->getTag().getName());
    }
    item->widget()->deleteLater();
}


QString InputLineTags::text()
{
    return ui->inputName->text();
}

void InputLineTags::setTags(QVector<TagRecord> tags)
{
    while (auto item =ui->tagStack->takeAt(0)) {
        item->widget()->deleteLater();
    }

    for (auto& tag : tags) {
        TagChip *chip = new TagChip(tag);
        ui->tagStack->addWidget(chip);
        connect(chip, &TagChip::clicked, chip, &TagChip::deleteLater);
    }
}

void InputLineTags::send()
{
    QVector<TagRecord> tags;
    int tagCount = ui->tagStack->count();
    for (int i=0; i<tagCount; i++) {
        if (TagChip *tagChip = dynamic_cast<TagChip*>(ui->tagStack->itemAt(i)->widget())) {
            tags.push_back(tagChip->getTag());
        }
    }
    emit returnPressed(tags);
}

void InputLineTags::enterEdit()
{
    QStringList stringList = db.getAllTagsNames();
    stringList.push_front("");
    model->setStringList(stringList);
}
