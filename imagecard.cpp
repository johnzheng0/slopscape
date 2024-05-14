#include "imagecard.h"
#include "ui_imagecard.h"

ImageCard::ImageCard(Database *dbIn, FileRecord fileIn, size_t maxWidth, QWidget *parent)
    : QWidget(parent)
    , file(fileIn)
    , selected(false)
    , ui(new Ui::ImageCard)
    , db(dbIn)
{
    ui->setupUi(this);

    init = QtConcurrent::run(&ImageCard::loadImage, this, maxWidth);
}

void ImageCard::resizeImage(size_t maxWidth)
{
    ui->image->setPixmap(image.scaledToWidth(maxWidth-16, Qt::SmoothTransformation));
}

ImageCard::~ImageCard()
{
    init.waitForFinished();
    delete ui;
}

void ImageCard::updateTags()
{
    QVector<TagRecord> tags = db->getFileTags(file);
    ui->tags->setText(QString::number(tags.size()));

    QStringList toolTipLabel;
    for (auto& tag : tags) {
        toolTipLabel.push_back(tag.getName());
    }
    ui->tagsContainer->setToolTip(toolTipLabel.join(" "));

    emit tagsUpdated();
}

void ImageCard::select(bool setting)
{
    selected = setting;
    if (selected)
        setStyleSheet("QWidget{background-color:rgb(64,64,64);}");
    else
        setStyleSheet("QWidget{background-color:rgb(40,40,40);}");
}

void ImageCard::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            emit doubleClicked(this);
        } else {
            emit clicked(this);
        }
    }
}

void ImageCard::loadImage(size_t maxWidth)
{
    QFileInfo fileInfo(file.getLocation());
    QString extension = fileInfo.suffix().toLower();

    if (extension == "mp4" || extension == "webm" ) {
        cv::VideoCapture video(file.getLocation().toStdString());
        if (video.isOpened()) {
            cv::Mat frame;
            video.read(frame);
            cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
            QImage videoFrame(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

            image = QPixmap::fromImage(videoFrame);
            ui->iconType->setPixmap(QPixmap(":/icons/icons/icon_video.svg"));
        }
    } else {
        image = QPixmap(file.getLocation());
        if (extension == "gif") {
            ui->iconType->setPixmap(QPixmap(":/icons/icons/icon_gif.svg"));
        }
    }
    ui->image->setPixmap(image.scaledToWidth(maxWidth-16, Qt::SmoothTransformation));

    updateTags();
}

