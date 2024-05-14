#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QVideoWidget>
#include <QMediaPlayer>

class VideoWidget : public QVideoWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr)
        : QVideoWidget(parent)
    {}

protected:
    void mousePressEvent(QMouseEvent*) override {
        emit clicked();
    }

signals:
    void clicked();
};

#endif // VIDEOWIDGET_H
