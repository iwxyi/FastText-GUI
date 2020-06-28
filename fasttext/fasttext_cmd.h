#ifndef FASTTEXT_UTIL_H
#define FASTTEXT_UTIL_H

#include <QString>
#include "fasttext_main.h"
#include "fasttext_util.h"

/**
 * 传入的字符串转 cmd 命令
 * 间接调用 FastTextUtil
 */
class FastTextCmd
{
public:
    /**
     * 不能简单的用 split(" ")
     * 要判断双引号
     * （反斜杠的双引号懒得判断了）
     */
    static void QString2Argv(QString qs, std::vector<std::string> *args)
    {
        int len = qs.length();
        QString temp = "";
        bool inQuote = false; // 是否在英文双引号中
        bool escape = false;  // 下个字符是否转义
        for (int i = 0; i < len; i++)
        {
            QChar ch = qs.at(i);
            /*if (ch == "\\")
            {
                if (!escape)
                {
                    escape = true;
                }
                else
                {
                    temp += "\\";
                    escape = false;
                }
            }
            else*/ if (ch == "\"" && !escape)
            {
                inQuote = !inQuote;
                if (!inQuote)
                {
                    (*args).push_back(temp.toStdString());
                    temp = "";
                }
            }
            else if (ch == ' ' && !inQuote)
            {
                if (!temp.isEmpty())
                {
                    (*args).push_back(temp.toStdString());
                    temp = "";
                }
            }
            else
            {
                temp += ch;
            }
        }
        if (!temp.isEmpty())
        {
            (*args).push_back(temp.toStdString());
        }
    }

    /**
     * （假装）调用 fastText 的命令行
     */
    static void exec(QString cmd)
    {
        std::vector<std::string> args;
        QString2Argv(cmd, &args);
        FastTextUtil::entrance(args);
    }
};

#endif // FASTTEXT_UTIL_H
