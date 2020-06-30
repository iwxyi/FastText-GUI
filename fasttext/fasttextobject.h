#ifndef FASTTEXTOBJECT_H
#define FASTTEXTOBJECT_H

#include <QObject>
#include <QFile>
#include <QTextCodec>
#include <QDir>
#include <QtConcurrent/QtConcurrent>
#include <QMetaType>
#include "fasttext_main.h"

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

    void predictLine(QString text, int k, PredictLineCallback callback);

private:
    QString convert2TxtFile(QString txt);
    static bool writeTextFile(QString path, QString text);
    static bool writeTextFile(QString path, QString text, QTextCodec *codec);

signals:
    void signalRunResultCallback(ResultCallback func);
    void signalRunPredictLineCallback(PredictLineCallback func, PredictResult rst);

public slots:

public:
    static QString fasttext_dir;

private:
    bool loading = true;
    std::string model_name; // 不包含后缀名
    FastText fastText;
};

#endif // FASTTEXTOBJECT_H
