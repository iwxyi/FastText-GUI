#include "fasttextobject.h"

QString FastTextObject::fasttext_dir = "predict/";

FastTextObject::FastTextObject(std::string model, QObject *parent) : FastTextObject(parent)
{
    loadModel(model);
}

FastTextObject::FastTextObject(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<PredictResult>("PredictResult");
    qRegisterMetaType<ResultCallback>("ResultCallback");
    qRegisterMetaType<PredictLineCallback>("PredictLineCallback");

    QDir dir(fasttext_dir);
    dir.mkpath(fasttext_dir);

    // 多线程结束后直接运行传进来的 lambda 会导致崩溃，需要使用信号槽的方式
    connect(this, &FastTextObject::signalRunResultCallback, this, [=](ResultCallback func) {
        if (func != nullptr)
            func();
    });
    connect(this, &FastTextObject::signalRunPredictLineCallback, this, [=](PredictLineCallback func, PredictResult rst) {
        if (func != nullptr)
            func(rst);
    });
}

void FastTextObject::loadModel(std::string name, ResultCallback callback)
{
    this->model_name = name;
    loading = true;
    QtConcurrent::run([=]{
        fastText.loadModel(name);
        loading = false;
        emit signalRunResultCallback(callback);
    });
}

void FastTextObject::train(std::string file, std::string model, ResultCallback callback)
{
    if (model == "")
        model = this->model_name;
    if (model == "")
        return ;
    QtConcurrent::run([&]{


    });
}

/**
 * 预测一行
 * 输出准确的多个标签及其概率
 * 如果是中文的话，记得先用 Jieba 分词
 */
void FastTextObject::predictLine(QString text, int k, PredictLineCallback callback)
{
    if (loading)
        return ;
    std::string path = convert2TxtFile(text).toStdString();
    QtConcurrent::run([=]{
        std::ifstream ifs;
        ifs.open(path);
        if (!ifs.is_open())
        {
            std::cerr << "Input file cannot be opened!" << std::endl;
            return ;
        }

        real threshold = 0.0;

        std::istream& in = ifs;
        PredictResult predictions;
        fastText.predictLine(in, predictions, k, threshold);
        ifs.close();
//        QFile(QString::fromStdString(path)).remove(); // 删除临时文件

        emit signalRunPredictLineCallback(callback, predictions);
    });
}

/**
 * 字符串转换为临时的文件，用来转换为fastText所需要的istream
 * 多线程预测结束后删除
 */
QString FastTextObject::convert2TxtFile(QString txt)
{
    int num = 0;
    do{
        num = rand() % 1000;
    } while (QFile().exists());
    QString path = fasttext_dir + QString::number(num);
    writeTextFile(path, txt);
    return path;
}

bool FastTextObject::writeTextFile(QString path, QString text)
{
    return writeTextFile(path, text, QTextCodec::codecForName(QByteArray("utf-8")));
}

bool FastTextObject::writeTextFile(QString path, QString text, QTextCodec *codec)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream text_stream(&file);
    text_stream.setCodec(codec);
    text_stream << text;
    file.close();
    return true;
}
