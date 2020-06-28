#include "fasttextutil.h"

FastTextUtil::FastTextUtil(QObject *parent) : QObject(parent)
{

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
 * @brief 获取分类结果
 * @param txt  输入的txt文件路径，或者字符串
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
                                "fasttext", "predict", "model", "txt", std::to_string(k)
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

/**
 * @brief 量化模型，减少体积
 * @param model 模型文件名（输入输出同名）
 */
void FastTextUtil::quantize(QString model)
{
    if (!model.endsWith(".bin"))
        model += ".bin";
    QtConcurrent::run([=]{
        std::vector<std::vector<std::pair<real, std::string>>> results =
                FastTextIf::predict(std::vector<std::string>{
                                "fasttext", "quantize", "-output", model.toStdString(), "-input", "x"
                            });
        emit signalQuantizeFinished();
    });
}

/**
 * txt转换为支持文本命令行的格式
 */
QString FastTextUtil::convertTxtFile(QString txt)
{
    if (!txt.endsWith(".txt"))
    {
        // TODO: 写入到文件
        QString name = "input_txt_temp.txt";

        return name;
    }
    // 带有空格的路径，需要包裹双引号
    else if (txt.contains(" "))
    {
        txt = "\"" + txt + "\"";
    }
    return txt;
}
