#ifndef FASTTEXTSTATIC_H
#define FASTTEXTSTATIC_H

#include <QObject>
#include <QStringList>
#include <QtConcurrent/QtConcurrent>
#include <QMetaType>
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
    void predict(QString txt, QString model);
    void predict(QString txt, QString model, int k);
    void quantize(QString model);

private:
    QString convertTxtFile(QString txt);
    static bool writeTextFile(QString path, QString text);
    static bool writeTextFile(QString path, QString text, QTextCodec *codec);

signals:
    void signalTrainFinished();
    void signalPredictOneFinished(QString label);
    void signalPredictsOneFinished(QStringList labels);
    void signalPredictFinished(QStringList labels);
    void signalPredictsFinished(QList<QStringList> labelss);
    void signalQuantizeFinished();
};

#endif // FASTTEXTSTATIC_H
