#ifndef SLIDER_H
#define SLIDER_H

#include <QSlider>
#include <QMouseEvent>
#include <QStyle>

class Slider : public QSlider
{
    Q_OBJECT
public:
    explicit Slider(QWidget *parent = nullptr)
        : QSlider{parent}
    {}

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            if (event->type() == QEvent::MouseButtonPress) {
                emit sliderPressed();
                setValue(QStyle::sliderValueFromPosition(minimum(), maximum(), event->position().x(), width()));
            }
        }
        QSlider::mousePressEvent(event);
    }

signals:
};

#endif // SLIDER_H
