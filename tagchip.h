#ifndef TAGCHIP_H
#define TAGCHIP_H

#include <QWidget>
#include "database.h"
#include <QEvent>
#include <QMouseEvent>

namespace Ui {
class TagChip;
}

class TagChip : public QWidget
{
    Q_OBJECT

public:
    explicit TagChip(TagRecord tag, QWidget *parent = nullptr);
    ~TagChip();
    TagRecord getTag() {return tag;}

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::TagChip *ui;
    TagRecord tag;

signals:
    void clicked(TagChip *tagChip);
};

#endif // TAGCHIP_H
