#ifndef VIEWERMAIN_H
#define VIEWERMAIN_H

#include <QWidget>
#include "database.h"
#include <QFileInfo>
#include <QResizeEvent>
#include <QDesktopServices>
#include <QtConcurrent/QtConcurrent>
#include "tagchip.h"
#include "dialogedit.h"

namespace Ui {
class ViewerMain;
}

class ViewerMain : public QWidget
{
    Q_OBJECT

public:
    explicit ViewerMain(QWidget *parent = nullptr);
    ~ViewerMain();

    void load(FileRecord file);
    void clear();

    const FileRecord getFile() {return file;}

    void editTags(QVector<TagRecord> dialogTags, DialogEdit::Modifier modifier);
    void updateTags();

private:
    Ui::ViewerMain *ui;
    QSettings settings;
    Database db;
    FileRecord file;
    QVector<TagRecord> tags;
    void loadFile(FileRecord file);
    void execEditTags();

private slots:
    void openInFileManager();


signals:
    void updated(QWidget* view);
    void tagClicked(TagChip *tag);
    void tagsEdited();
    void autoTagClicked(FileRecord file);
};

#endif // VIEWERMAIN_H
