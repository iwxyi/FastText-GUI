#ifndef FASTTEXTOBJECT_H
#define FASTTEXTOBJECT_H

#include <QObject>
#include <QFile>
#include <QTextCodec>
#include <QDir>
#include <QtConcurrent/QtConcurrent>
#include <QMetaType>
#include "fasttext_main.h"

const QString FASTTEXT_MODEL_DIR = "model/";
const QString FASTTEXT_TEMP_DIR = "predict/";
const QString FASTTEXT_TRAIN_DIR = "train/";

typedef std::vector<std::pair<real, std::string>> PredictResult;
typedef std::function<void()> ResultCallback;
typedef std::function<void(std::vector<std::pair<real, std::string>>)> PredictLineCallback;
typedef std::function<void(std::string)> PredictOneCallback;
typedef std::function<void(std::vector<std::vector<std::pair<real, std::string>>>)> PredictCallback;

/**
 * 针对一个 model 多次预测的工具类
 */
class FastTextObject : public QObject
{
    Q_OBJECT
public:
    FastTextObject(std::string model, QObject *parent = nullptr);
    FastTextObject(QObject *parent = nullptr);

    void loadModel(std::string name, ResultCallback callback = nullptr);

    void train(std::string file, std::string model = "", ResultCallback callback = nullptr);

    void quantize(ResultCallback callback = nullptr);

    void predictLine(QString text, int k, PredictLineCallback callback);

protected:
    void train(const std::vector<std::string> args) const;

private:
    QString convert2TxtFile(QString txt);
    static bool writeTextFile(QString path, QString text);
    static bool writeTextFile(QString path, QString text, QTextCodec *codec);

signals:
    void signalRunResultCallback(ResultCallback func);
    void signalRunPredictLineCallback(PredictLineCallback func, PredictResult rst);

public slots:

public:
    static QString fasttext_dir; // 文件目录：运行路径/predict/

private:
    bool loading = true;
    std::string model_name; // 不包含后缀名
    FastText fastText;
};

#endif // FASTTEXTOBJECT_H
