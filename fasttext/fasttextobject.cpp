#include "fasttextobject.h"

QString FastTextObject::fasttext_dir = "predict/";

FastTextObject::FastTextObject(QString model, QObject *parent) : FastTextObject(parent)
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
    dir.mkpath(fasttext_dir + FASTTEXT_TEMP_DIR);

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

bool FastTextObject::isValid()
{
    return loading == false;
}

FastText* FastTextObject::getCore()
{
    return &this->fastText;
}

/**
 * 加载模型
 */
void FastTextObject::loadModel(QString name, ResultCallback callback)
{
    QFile file(name);
    if (!file.exists())
    {
        qDebug() << "模型：" << name << "不存在";
        return ;
    }

    this->model_name = name;
    loading = true;
    QtConcurrent::run([=]{
        fastText.loadModel(name.toStdString());
        loading = false;
        emit signalRunResultCallback(callback);
    });
}

/**
 * 训练模型
 * 建议训练后的 callback 中重新加载
 * model 不包含后缀名
 */
void FastTextObject::train(QString file, QString model, ResultCallback callback)
{
    if (model == "")
        model = this->model_name;
    if (model == "")
        return ;
    loading = true;
    QtConcurrent::run([=]{
        // Args 有点难提取，干脆继续这个形式了
        train(std::vector<std::string>{"fasttext", "supervised", "-input", file.toStdString(), "-output", model.toStdString()});
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

    QString name = model_name;
    if (name.endsWith(".ftz"))
    {
        qDebug() << "已经是量化模型，无需再量化";
        return ;
    }
    else if (name.endsWith(".bin"))
    {
        name = name.left(name.length()-4);
    }
    std::vector<std::string> args{"fasttext", "quantize", "-output", name.toStdString(), "-input", name.toStdString()};

    QtConcurrent::run([=]{
        Args a = Args();
        fastText.quantize(a);
        fastText.saveModel((name + ".ftz").toStdString());
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

    QtConcurrent::run([=]{
        auto predictions = predictLineSync(text, k);
        emit signalRunPredictLineCallback(callback, predictions);
    });
}

PredictResult FastTextObject::predictLineSync(QString text, int k)
{
    std::string path = convert2TxtFile(text).toStdString();
    std::ifstream ifs;
    ifs.open(path);
    if (!ifs.is_open())
    {
        std::cerr << "Input file cannot be opened!" << std::endl;
        return PredictResult{};
    }

    real threshold = 0.0;

    std::istream& in = ifs;
    PredictResult predictions;
    fastText.predictLine(in, predictions, k, threshold);
    ifs.close();
    QFile(QString::fromStdString(path)).remove(); // 删除临时文件
    return predictions;
}

QString FastTextObject::predictOneSync(QString text)
{
    auto result = predictLineSync(text);
    if (result.size() > 0)
    {
        auto rst = QString::fromStdString(result.at(0).second);
        if (rst.startsWith("__label__"))
            rst = rst.right(rst.length()-9);
        return rst;
    }
    return "";
}

Vector FastTextObject::getSentenceVector(QString sent)
{
    QString path = convert2TxtFile(sent);

    Vector svec(fastText.getDimension());
    std::ifstream ifs;
    ifs.open(path.toStdString());
    if (!ifs.is_open())
    {
        qDebug() << "getSentenceVecotr 读取文件失败";
        return svec;
    }
    if (ifs.peek() != EOF) // 读取一行
    {
        fastText.getSentenceVector(ifs, svec);
    }
    return svec;
}

QString FastTextObject::getFirst(const PredictResult &result) const
{
    QString s = QString::fromStdString(result.at(0).second);
    if (s.startsWith("__label__"))
        s = s.right(s.length() - 9);
    return s;
}

/**
 * 向量的字符串转向量
 */
QList<float> FastTextObject::string2Vector(QString text)
{
    QList<float> v;
    QStringList sl = text.split(QRegularExpression(",\\s*"));
    foreach (auto s, sl) {
        bool ok;
        auto f = s.toFloat(&ok);
        if (!ok)
            continue;
        v.push_back(f);
    }
    return v;
}

/**
 * 计算两个向量之间的余弦相似度
 */
double FastTextObject::calcVectorSimilar(QList<float> l1, QList<float> l2)
{
    if (l1.size() != l2.size() || l1.size() == 0)
        return 0;
    int len = l1.size();

    float fz = 0;
    for (int i = 0; i < len; i++)
    {
        fz += l1.at(i) * l2.at(i);
    }

    float fm = 0, fm1 = 0, fm2 = 0;
    for (int i = 0; i < len; i++)
    {
        fm1 += l1.at(i) * l1.at(i);
        fm2 += l2.at(i) * l2.at(i);
    }

    return fz / (sqrt(fm1) * sqrt(fm2));
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
    QString path = fasttext_dir + FASTTEXT_TEMP_DIR + QString::number(num);
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
