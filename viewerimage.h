#ifndef VIEWERIMAGE_H
#define VIEWERIMAGE_H

#include <QWidget>
#include "database.h"
#include <QResizeEvent>
#include <QFileInfo>

namespace Ui {
class ViewerImage;
}

class ViewerImage : public QWidget
{
    Q_OBJECT

public:
    explicit ViewerImage(QWidget *parent = nullptr);
    ~ViewerImage();

    void load(FileRecord file);
    void clear();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::ViewerImage *ui;
    QPixmap image;
};

#endif // VIEWERIMAGE_H
