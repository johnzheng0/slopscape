#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>
#include <QFocusEvent>
#include <QKeyEvent>

class LineEdit : public QLineEdit
{
    Q_OBJECT

public:
    LineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {
        this->setFocusPolicy(Qt::ClickFocus);
    }
    QSize textSize() {
        return fontMetrics().size(0, text());
    }

protected:
    void focusInEvent(QFocusEvent *event) override
    {
        emit focusedIn();
        QLineEdit::focusInEvent(event);
    }
    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->key() == Qt::Key_Backspace && text().isEmpty()) {
            emit emptyBackspace();
        }
        if (textSize().width() >= width()) {
            emit exceedWidth();
        }
        QLineEdit::keyPressEvent(event);
    }

signals:
    void focusedIn();
    void emptyBackspace();
    void exceedWidth();
};

#endif // LINEEDIT_H
