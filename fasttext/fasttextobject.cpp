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

    QDir dir;
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

/**
 * 加载模型
 */
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

/**
 * 训练模型
 * 建议训练后的 callback 中重新加载
 */
void FastTextObject::train(std::string file, std::string model, ResultCallback callback)
{
    if (model == "")
        model = this->model_name;
    if (model == "")
        return ;
    loading = true;
    QtConcurrent::run([=]{
        // Args 有点难提取，干脆继续这个形式了
        train(std::vector<std::string>{"fasttext", "supervised", "-input", file, "-output", model});
        loading = false;
        emit signalRunResultCallback(callback);
    });
}

void FastTextObject::quantize(ResultCallback callback)
{
    if (loading)
    {
        qDebug() << "loading model";
        return ;
    }

    std::string name = model_name;
    if (name.find(".bin") > 0)
        name = name.substr(0, name.length()-4);
    std::vector<std::string> args{"fasttext", "quantize", "-output", name, "-input", name};

    QtConcurrent::run([=]{
        Args a = Args();
        fastText.quantize(a);
        fastText.saveModel(name + ".ftz");
        emit signalRunResultCallback(callback);
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
    {
        qDebug() << "loading model";
        return ;
    }
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
        QFile(QString::fromStdString(path)).remove(); // 删除临时文件

        emit signalRunPredictLineCallback(callback, predictions);
    });
}

/**
 * 调用 fastText 内部的 API
 */
void FastTextObject::train(const std::vector<std::string> args) const
{
    Args a = Args();
    a.parseArgs(args);
    std::shared_ptr<FastText> fasttext = std::make_shared<FastText>();
    std::string outputFileName;

    if (a.hasAutotune() &&
            a.getAutotuneModelSize() != Args::kUnlimitedModelSize) {
        outputFileName = a.output + ".ftz";
    } else {
        outputFileName = a.output + ".bin";
    }
    std::ofstream ofs(outputFileName);
    if (!ofs.is_open()) {
        throw std::invalid_argument(
                    outputFileName + " cannot be opened for saving.");
    }
    ofs.close();
    if (a.hasAutotune()) {
        Autotune autotune(fasttext);
        autotune.train(a);
    } else {
        fasttext->train(a);
    }
    fasttext->saveModel(outputFileName);
    fasttext->saveVectors(a.output + ".vec");
    if (a.saveOutput) {
        fasttext->saveOutput(a.output + ".output");
    }
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
