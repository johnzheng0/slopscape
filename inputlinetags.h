#ifndef INPUTLINETAGS_H
#define INPUTLINETAGS_H

#include <QWidget>
#include <QCompleter>
#include <QStringListModel>
#include "tagchip.h"
#include "database.h"
#include "QApplication"
#include <QLineEdit>
#include "lineedit.h"

namespace Ui {
class InputLineTags;
}

class InputLineTags : public QWidget
{
    Q_OBJECT

public:
    explicit InputLineTags(QWidget *parent = nullptr);
    ~InputLineTags();
    QString text();
    void setTags(QVector<TagRecord>);
    void send();

private:
    Ui::InputLineTags *ui;
    QStringListModel *model;
    QStringListModel *emptyModel;
    QCompleter *completer;
    Database db;
    QVector<TagChip*> tagChips;

    void enterEdit();
    void handleEdit(const QString text);
    void handleReturn();
    void handleBackspace();
signals:
    void returnPressed(QVector<TagRecord> tags);
};

#endif // INPUTLINETAGS_H
