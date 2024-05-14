#ifndef DIALOGCONFIRM_H
#define DIALOGCONFIRM_H

#include <QDialog>

namespace Ui {
class DialogConfirm;
}

class DialogConfirm : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConfirm(QString message, QString acceptLabel = "Ok", QString rejectlabel = "Cancel", QWidget *parent = nullptr);
    ~DialogConfirm();

private:
    Ui::DialogConfirm *ui;
};

#endif // DIALOGCONFIRM_H
