#ifndef INPUTTAGS_H
#define INPUTTAGS_H

#include <QWidget>
#include <QCompleter>
#include <QStringListModel>
#include "tagchip.h"
#include "database.h"
#include "QApplication"
#include <QLineEdit>
#include "lineedit.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QLayout>

namespace Ui {
class InputTags;
}

class InputTags : public QWidget
{
    Q_OBJECT

public:
    explicit InputTags(QWidget *parent = nullptr);
    ~InputTags();
    QVector<TagRecord> getTags();
    void setTags(QVector<TagRecord> tags);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::InputTags *ui;
    QStringListModel *model;
    QCompleter *completer;
    Database db;
    QVector<TagChip*> tagChips;

    void doLayout();
    void deleteChip(TagChip *chip);

    void enterEdit();
    void handleEdit(const QString text);
    void handleReturn();
    void handleBackspace();
    void handleExceedWidth();

signals:
    void returnPressed();
};

#endif // INPUTTAGS_H
