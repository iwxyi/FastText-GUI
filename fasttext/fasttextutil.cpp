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
        emit signalFinished();
    });
}

QString FastTextUtil::predict(QString txt, QString model)
{
    txt = convertTxtFile(txt);
    if (!model.endsWith(".bin"))
        model += ".bin";
    QtConcurrent::run([=]{
        FastTextCmd::exec("fasttext predic "+model+" "+txt);
        // TODO: 想办法获取输出
        QStringList list;
        if (list.size() == 0)
            emit signalPredict("");
        else
            emit signalPredict(list.first());
    });
}

/**
 * @brief 获取分类结果
 * @param txt  输入的txt文件路径，或者字符串
 * @param mode 使用的模型
 * @param k    标签个数（默认为1）
 * @return
 */
void FastTextUtil::predict(QString txt, QString model, int k)
{
    txt = convertTxtFile(txt);
    if (!model.endsWith(".bin"))
        model += ".bin";
    QtConcurrent::run([=]{
        FastTextCmd::exec("fasttext predic "+model+" "+txt+" "+QString::number(k));
        // TODO: 想办法获取输出
        QStringList list;
        emit signalPredict(list);
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
