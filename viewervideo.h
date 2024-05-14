#ifndef VIEWERVIDEO_H
#define VIEWERVIDEO_H

#include <QWidget>
#include <QtMultimediaWidgets/QtMultimediaWidgets>
#include <QResizeEvent>
#include "videowidget.h"
#include <QAudioOutput>
#include "database.h"
#include <QtMultimedia>
#include <QFileInfo>

namespace Ui {
class ViewerVideo;
}

class ViewerVideo : public QWidget
{
    Q_OBJECT

public:
    explicit ViewerVideo(QWidget *parent = nullptr);
    ~ViewerVideo();

    void load(FileRecord file);
    void clear();

private:
    Ui::ViewerVideo *ui;
    QMediaPlayer *player;
    VideoWidget *videoWidget;
    QAudioOutput *audioOutput;
    int state;

    void onPositionChange(size_t position);
    void onSliderPress();
    void onSliderRelease();
    void onVideoClick();
    void onAudioAvailability(bool available);
};

#endif // VIEWERVIDEO_H
