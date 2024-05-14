#include "viewervideo.h"
#include "ui_viewervideo.h"

ViewerVideo::ViewerVideo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ViewerVideo)
    , player(new QMediaPlayer)
    , videoWidget(new VideoWidget)
    , audioOutput(new QAudioOutput)
{
    ui->setupUi(this);

    // set video player parameters
    ui->videoPort->layout()->addWidget(videoWidget);
    player->setVideoOutput(videoWidget);
    player->setAudioOutput(audioOutput);
    player->audioOutput()->setMuted(true);
    player->setLoops(-1);

    // connect slider functionality and player functionality
    connect(player, &QMediaPlayer::durationChanged, ui->scrubber, &QSlider::setMaximum);
    connect(player, &QMediaPlayer::positionChanged, this, &ViewerVideo::onPositionChange);
    connect(ui->scrubber, &QSlider::sliderMoved, player, &QMediaPlayer::setPosition);
    connect(ui->scrubber, &QSlider::sliderPressed, this, &ViewerVideo::onSliderPress);
    connect(ui->scrubber, &QSlider::sliderReleased, this, &ViewerVideo::onSliderRelease);

    // connect buttons to player functionality
    connect(videoWidget, &VideoWidget::clicked, this, &ViewerVideo::onVideoClick);
    connect(ui->buttonMute, &QToolButton::clicked, player->audioOutput(), &QAudioOutput::setMuted);
}

ViewerVideo::~ViewerVideo()
{
    delete player;
    delete videoWidget;
    delete ui;
}

void ViewerVideo::load(FileRecord file)
{
    // check if file is gif
    QFileInfo fileInfo(file.getLocation());
    QString extension = fileInfo.suffix().toLower();
    if (extension == "gif") {
        ui->controlBar->hide();
    } else {
        ui->controlBar->show();
    }

    // load file
    player->setSource(QUrl::fromLocalFile(file.getLocation()));
    player->play();
    state = QMediaPlayer::PlayingState;
}

void ViewerVideo::clear()
{
    player->stop();
}

void ViewerVideo::onPositionChange(size_t position)
{
    if (!ui->scrubber->isSliderDown())
        ui->scrubber->setSliderPosition(position);
    size_t valueStart = ui->scrubber->value();
    size_t valueEnd = ui->scrubber->maximum() - valueStart;
    QString sec, min, hr;
    sec = QString::number(valueStart/1000);
    min = QString::number(valueStart/60000);
    hr = QString::number(valueStart/360000);
    ui->labelStart->setText(hr+":"+min+":"+sec);
    sec = QString::number(valueEnd/1000);
    min = QString::number(valueEnd/60000);
    hr = QString::number(valueEnd/360000);
    ui->labelEnd->setText(hr+":"+min+":"+sec);
}

void ViewerVideo::onSliderPress()
{
    player->pause();
    player->setPosition(ui->scrubber->sliderPosition());
}

void ViewerVideo::onSliderRelease()
{
    if (state == QMediaPlayer::PlayingState)
        player->play();
}

void ViewerVideo::onVideoClick()
{
    if (player->isPlaying() == QMediaPlayer::PlayingState) {
        player->pause();
    } else {
        player->play();
    }
    state = player->isPlaying();
}
