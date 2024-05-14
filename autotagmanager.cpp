#include "autotagmanager.h"
#include "ui_autotagmanager.h"

AutoTagManager::AutoTagManager(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AutoTagManager)
    , changed(false)
{
    ui->setupUi(this);
    hide();
}

AutoTagManager::~AutoTagManager()
{
    save();
    delete ui;
}

void AutoTagManager::train()
{
    if (trainThread.isRunning()) return;

    trainThread = QtConcurrent::run([&]() {
        Database db;

        // get files and tags from database
        QVector<FileRecord> files = db.getFilesWithTags();
        if (files.isEmpty()) return;
        tags = db.getAllTags();
        tags.remove(0, 3);

        show();

        for (FileRecord& file : files) {
            std::swap(file, files[rand() % files.size()]);
        }

        // generate training matrices for data and labels
        cv::Mat trainData, trainLabels;
        QVector<QFuture<QPair<cv::Mat, cv::Mat>>> threads;
        for (FileRecord& file : files) {
            threads.push_back(QtConcurrent::run([&]() {
                cv::Mat image = getImage(file.getLocation());
                if (image.empty())
                    return QPair<cv::Mat, cv::Mat>(image, image);
                cv::resize(image, image, cv::Size(16,16));
                image = image.reshape(1,1);

                QVector<TagRecord> fileTags = db.getFileTags(file);
                cv::Mat labels = cv::Mat::zeros(1, tags.size(), CV_32S);
                for (int i=0; i<labels.cols; i++) {
                    if (fileTags.contains(tags[i])) {
                        labels.at<int>(0, i) = 1;
                    }
                }

                return QPair<cv::Mat, cv::Mat>(image, labels);
            }));
        }
        for (auto& thread : threads) {
            QPair<cv::Mat, cv::Mat> results = thread.result();
            if (results.first.empty())
                continue;
            trainData.push_back(results.first);
            trainLabels.push_back(results.second);
        }
        trainData.convertTo(trainData, CV_32F);
        trainLabels.convertTo(trainLabels, CV_32F);

        // create neural network
        model = cv::ml::ANN_MLP::create();
        // cv::Mat layers = (cv::Mat_<int>(1, 4) << trainData.cols, trainData.cols * 0.5, trainLabels.cols * 0.5, trainLabels.cols);
        cv::Mat layers = (cv::Mat_<int>(1, 3) << trainData.cols, trainLabels.cols * 0.5, trainLabels.cols);
        model->setLayerSizes(layers);

        // set neural network term criteria
        cv::TermCriteria term_criteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 10000, 0.0001);
        model->setTermCriteria(term_criteria);

        // set neural network train method
        model->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM, 1.0, 1.0);
        model->setTrainMethod(cv::ml::ANN_MLP::BACKPROP);
        model->setBackpropMomentumScale(0.4);
        model->setBackpropWeightScale(0.1);

        // train neural network
        cv::Ptr<cv::ml::TrainData> trainDataset = cv::ml::TrainData::create(trainData, cv::ml::ROW_SAMPLE, trainLabels);
        trainDataset->setTrainTestSplitRatio(0.8);
        model->train(trainDataset);

        cv::Mat predictions;
        model->predict(trainData, predictions);

        hide();
        changed = true;
    });
}

QVector<TagRecord> AutoTagManager::predict(FileRecord file)
{
    // return if no model is trained or exists
    if (model.empty()) {
        load();
        if (model.empty())return QVector<TagRecord>();
    }

    // initialize image features
    QVector<TagRecord> predictedTags;
    cv::Mat data;
    cv::Mat image = getImage(file.getLocation());
    if (image.empty()) return predictedTags; // return nothing if image fails to load
    cv::resize(image, image, cv::Size(16,16));
    image = image.reshape(1,1);
    image.convertTo(image,CV_32F);

    // predict tags
    cv::Mat prediction;
    model->predict(image, prediction);
    for (int i=0; i<tags.size(); i++) {
        if (prediction.at<float>(i) >= 0.9) {
            predictedTags.push_back(tags[i]);
        }
    }

    // return predicted tags
    return predictedTags;
}

void AutoTagManager::save()
{
    // save model
    if (!changed) return;
    std::string dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
    model->save(dataDir + "/model.xml");
}

void AutoTagManager::load()
{
    // load model
    if (tags.empty()) {
        Database db;
        tags = db.getAllTags();
        tags.remove(0, 3);
    }
    std::string dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
    std::string modelPath = dataDir + "/model.xml";
    QFileInfo fileInfo(QString::fromStdString(modelPath));
    if (!fileInfo.exists()) return;
    model = cv::ml::ANN_MLP::load(modelPath);
}

Mat AutoTagManager::getImage(QString location)
{
    // check if file exists
    QFile file(location);
    if (!file.exists()) return cv::Mat();

    // get image contents and return it
    QFileInfo fileInfo(location);
    QString extension = fileInfo.suffix().toLower();
    if (extension == "mp4" || extension == "webm") {
        cv::VideoCapture video(location.toStdString());
        int middleFrameIndex = video.get(cv::CAP_PROP_FRAME_COUNT) / 2;
        video.set(cv::CAP_PROP_POS_FRAMES, middleFrameIndex);

        cv::Mat frame;
        video.read(frame);
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        return frame;
    } else if (extension == "gif") {
        QMovie gifMovie(location);
        gifMovie.start();

        while (!gifMovie.currentPixmap().isNull()) {
            QImage image = gifMovie.currentPixmap().toImage();
            cv::Mat frame(image.height(), image.width(), CV_8UC4, image.bits(), image.bytesPerLine());
            cv::cvtColor(frame, frame, cv::COLOR_RGBA2GRAY);
            return frame;
        }
        return cv::Mat();
    } else {
        return imread(location.toStdString(), cv::IMREAD_GRAYSCALE);
    }
}
