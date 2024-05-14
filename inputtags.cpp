#include "inputtags.h"
#include "ui_inputtags.h"

InputTags::InputTags(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InputTags)
    , model(new QStringListModel())
    , completer(new QCompleter(model))
    , db(Database())
{
    ui->setupUi(this);

    this->setFocusProxy(ui->inputName);

    completer->setCompletionMode(QCompleter::InlineCompletion);
    ui->inputName->setCompleter(completer);

    connect(ui->inputName, &LineEdit::focusedIn, this, &InputTags::enterEdit);
    connect(ui->inputName, &LineEdit::returnPressed, this, &InputTags::handleReturn);
    connect(ui->inputName, &LineEdit::emptyBackspace, this, &InputTags::handleBackspace);
    connect(ui->inputName, &LineEdit::textChanged, this, &InputTags::handleEdit);
    connect(ui->inputName, &LineEdit::exceedWidth, this, &InputTags::handleExceedWidth);

    doLayout();
}

InputTags::~InputTags()
{
    delete ui;
}

QVector<TagRecord> InputTags::getTags()
{
    QVector<TagRecord> tags;
    for (auto& chip : tagChips) {
        tags.push_back(chip->getTag());
    }
    return tags;
}

void InputTags::setTags(QVector<TagRecord> tags)
{
    for (auto& tag : tags) {
        TagChip *chip = new TagChip(tag, ui->container);
        chip->show();
        chip->setGeometry(chip->layout()->itemAt(0)->geometry());
        connect(chip, &TagChip::clicked, this, &InputTags::deleteChip);
        tagChips.push_back(chip);
    }
    ui->inputName->clear();
    doLayout();
}

void InputTags::resizeEvent(QResizeEvent*)
{
    doLayout();
}

void InputTags::doLayout()
{
    int x = 2;
    int y = 2;
    int maxRowY = 0;

    for (auto& chip : tagChips) {
        if (chip->height() > maxRowY) {
            maxRowY = chip->height();
        }
        if (x+chip->width() > width()) {
            x = 2;
            y += maxRowY + 2;
            maxRowY = 0;
        }
        chip->setGeometry(x, y, chip->width(), chip->height());
        x += chip->width() + 2;
    }
    if (x+ui->inputName->textSize().width() > width()) {
        x = 2;
        y += maxRowY + 2;
    }
    ui->inputName->setGeometry(x, y, ui->inputName->width(), ui->inputName->height());
}

void InputTags::deleteChip(TagChip *chip)
{
    tagChips.removeOne(chip);
    delete chip;
    doLayout();
}

void InputTags::enterEdit()
{
    QStringList stringList = db.getAllTagsNames();
    stringList.push_front("");
    model->setStringList(stringList);
}

void InputTags::handleEdit(const QString text)
{
    QStringList tagNames = text.split(" ");
    if (!tagNames.last().isEmpty()) {
        if (ui->inputName->textSize().width() + ui->inputName->x() > ui->container->width()) {
            doLayout();
        }
        return;
    }

    tagNames.removeAll("");

    for (auto& tagName : tagNames) {
        TagChip *chip = new TagChip(TagRecord(tagName), ui->container);
        chip->show();
        chip->setGeometry(chip->layout()->itemAt(0)->geometry());
        connect(chip, &TagChip::clicked, this, &InputTags::deleteChip);
        tagChips.push_back(chip);
    }
    ui->inputName->clear();

    doLayout();
}

void InputTags::handleReturn()
{
    if (ui->inputName->hasSelectedText()) {
        QKeyEvent complete(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
        QApplication::sendEvent(ui->inputName, &complete);
    } else if (!ui->inputName->text().isEmpty()) {
        handleEdit(ui->inputName->text()+" ");
    } else {
        emit returnPressed();
    }
}

void InputTags::handleBackspace()
{
    if (tagChips.isEmpty()) return;

    TagChip *chip = tagChips.last();
    ui->inputName->setText(chip->getTag().getName());
    deleteChip(chip);
}

void InputTags::handleExceedWidth()
{
    QRect geo = ui->inputName->geometry();
    ui->inputName->setGeometry(geo.x(),geo.y(),geo.width()+20,geo.height());
}



