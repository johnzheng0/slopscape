#ifndef DIALOGEDIT_H
#define DIALOGEDIT_H

#include <QDialog>
#include "database.h"
#include <QKeyEvent>

namespace Ui {
class DialogEdit;
}

class DialogEdit : public QDialog
{
    Q_OBJECT

protected:
    void keyPressEvent(QKeyEvent *event) override;

public:
    enum Modifier {
        Add,
        Remove,
        Overwrite
    };

    explicit DialogEdit(QWidget *parent = nullptr);
    ~DialogEdit();
    QVector<TagRecord> getTags();
    void setTags(QVector<TagRecord> tags);
    void setModifier(Modifier modifier);
    DialogEdit::Modifier getModifier();

private:
    Ui::DialogEdit *ui;
};

#endif // DIALOGEDIT_H
