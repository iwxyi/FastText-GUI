#include "fasttext_static.h"

FastTextUtil::FastTextUtil(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<QList<QStringList>>("QList<QStringList>");
}

/**
 * @brief 训练模型
 * @param txt   输入的txt文件路径（相对或绝对）
 * @param model 输出的model文件名字
 */
void FastTextUtil::train(QString txt, QString model)
{
    txt = convertTxtFile(txt);
    QtConcurrent::run([=]{
        FastTextCmd::exec("fasttext supervised -input "+txt+" -output "+model);
        emit signalTrainFinished();
    });
}

void FastTextUtil::predictOne(QString txt, QString model)
{
    txt = convertTxtFile(txt);
    if (!model.endsWith(".bin"))
        model += ".bin";
    QtConcurrent::run([=]{
        std::vector<std::vector<std::pair<real, std::string>>> results =
                FastTextIf::predict(std::vector<std::string>{
                                "fasttext", "predict", model.toStdString(), txt.toStdString()
                            });
        QString rst;
        if (results.size() > 0 && results.front().size() > 0)
        {
            rst = QString::fromStdString(results.front().front().second);
            if (rst.startsWith("__label__"))
                rst = rst.right(rst.length()-9);
        }
        emit signalPredictOneFinished(rst);
    });
}

/**
 * @brief 获取一个数据的分类结果
 * @param txt  输入的txt文件路径，或者字符串（如果有多条，则只取一条）
 * @param mode 使用的模型
 * @param k    标签个数（默认为1）
 * @return
 */
void FastTextUtil::predictOne(QString txt, QString model, int k)
{
    txt = convertTxtFile(txt);
    if (!model.endsWith(".bin"))
        model += ".bin";
    QtConcurrent::run([=]{
        std::vector<std::vector<std::pair<real, std::string>>> results =
                FastTextIf::predict(std::vector<std::string>{
                                "fasttext", "predict", model.toStdString(), txt.toStdString(), std::to_string(k)
                            });
        QStringList list;
        foreach (auto r, results.front()) // 只遍历第一行
        {
            QString s = QString::fromStdString(r.second);
            if (s.startsWith("__label__"))
                s = s.right(s.length()-9);
            list << s;
        }
        emit signalPredictsOneFinished(list);
    });
}

void FastTextUtil::predict(QString txt, QString model)
{
    txt = convertTxtFile(txt);
    if (!model.endsWith(".bin"))
        model += ".bin";
    QtConcurrent::run([=]{
        std::vector<std::vector<std::pair<real, std::string>>> results =
                FastTextIf::predict(std::vector<std::string>{
                                "fasttext", "predict", model.toStdString(), txt.toStdString()
                            });
        QStringList list;
        foreach (auto result, results)
        {
            if (result.size() > 0)
            {
                QString s = QString::fromStdString(result.front().second);
                if (s.startsWith("__label__"))
                    s = s.right(s.length()-9);
                list << s;
            }
        }
        emit signalPredictFinished(list);
    });
}

void FastTextUtil::predict(QString txt, QString model, int k)
{
    txt = convertTxtFile(txt);
    if (!model.endsWith(".bin"))
        model += ".bin";
    QtConcurrent::run([=]{
        std::vector<std::vector<std::pair<real, std::string>>> results =
                FastTextIf::predict(std::vector<std::string>{
                                "fasttext", "predict", model.toStdString(), txt.toStdString(), std::to_string(k)
                            });
        QList<QStringList> lists;
        foreach (auto result, results)
        {
            QStringList list;
            foreach (auto r, result) // 只遍历第一行
            {
                QString s = QString::fromStdString(r.second);
                if (s.startsWith("__label__"))
                    s = s.right(s.length()-9);
                list << s;
            }
            lists << list;
        }
        emit signalPredictsFinished(lists);
    });
}

/**
 * @brief 量化模型，减少体积
 * model.bin => model.ftz
 * @param model 模型文件名（输入输出同名）
 */
void FastTextUtil::quantize(QString model)
{
    QtConcurrent::run([=]{
        FastTextIf::quantize(std::vector<std::string>{
                                "fasttext", "quantize", "-output", model.toStdString(), "-input", model.toStdString()
                            });
        emit signalQuantizeFinished();
    });
}

/**
 * txt转换为支持文本命令行的格式
 */
QString FastTextUtil::convertTxtFile(QString txt)
{
    if (!txt.endsWith(".txt") || txt.contains("\n"))
    {
        // TODO: 写入到文件
        QString name = "input_txt_temp.txt";
        writeTextFile(name, txt);
        return name;
    }
    // 带有空格的路径，需要包裹双引号
    else if (txt.contains(" "))
    {
        txt = "\"" + txt + "\"";
    }
    return txt;
}

bool FastTextUtil::writeTextFile(QString path, QString text)
{
    return writeTextFile(path, text, QTextCodec::codecForName(QByteArray("utf-8")));
}

bool FastTextUtil::writeTextFile(QString path, QString text, QTextCodec *codec)
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
