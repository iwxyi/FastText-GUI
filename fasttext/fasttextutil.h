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
    QString predict(QString txt, QString model);
    QStringList predict(QString txt, QString model, int k);

private:
    QString convertTxtFile(QString txt);

signals:
    void signalFinished();
    void signalPredict(QString label);
    void signalPredict(QStringList labels);

public slots:

private:

};

#endif // FASTTEXTUTIL_H
