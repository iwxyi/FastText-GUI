#ifndef FASTTEXTUTIL_H
#define FASTTEXTUTIL_H

#include <QObject>
#include <QStringList>
#include <QtConcurrent/QtConcurrent>
#include "fasttext_cmd.h"

/**
 * fastText 标签分类工具类
 * 功能：
 * - 训练模型
 * - 获取分类
 */
class FastTextUtil : public QObject
{
    Q_OBJECT
public:
    FastTextUtil(QObject *parent = nullptr);

    void train(QString txt, QString model);
    void predictOne(QString txt, QString model);
    void predictOne(QString txt, QString model, int k);
    void quantize(QString model);

private:
    QString convertTxtFile(QString txt);

signals:
    void signalTrainFinished();
    void signalPredictOneFinished(QString label);
    void signalPredictsOneFinished(QStringList labels);
    void signalQuantizeFinished();

public slots:

private:

};

#endif // FASTTEXTUTIL_H
