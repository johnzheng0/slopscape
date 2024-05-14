#ifndef AUTOTAGMANAGER_H
#define AUTOTAGMANAGER_H

#include <QWidget>
#include "database.h"
#include <QStandardPaths>
#include <opencv2/opencv.hpp>
#include <QtConcurrent/QtConcurrent>
#include <QMovie>
#include <QtConcurrent/QtConcurrent>

using namespace cv;

namespace Ui {
class AutoTagManager;
}

class AutoTagManager : public QWidget
{
    Q_OBJECT

public:
    explicit AutoTagManager(QWidget *parent = nullptr);
    ~AutoTagManager();

    void train();
    QVector<TagRecord> predict(FileRecord file);

    void save();
    void load();

private:
    Ui::AutoTagManager *ui;
    QVector<TagRecord> tags;
    cv::Ptr<cv::ml::ANN_MLP> model;
    Mat getImage(QString location);
    QFuture<void> trainThread;
    QVector<QFuture<void>> predictThreads;
    bool changed;
};

#endif // AUTOTAGMANAGER_H
