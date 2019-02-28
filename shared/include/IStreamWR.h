#ifndef ITEMDATAWRITER_H
#define ITEMDATAWRITER_H

#include "BlackboardHeader.h"
#include <QDataStream>
#include <QByteArray>

/**
 * @brief The IStreamW class
 *      数据流写入接口
 */
class NSB_BLACKBOARD_EXPORT IStreamW
{
public:
    virtual ~IStreamW(){}

    /**
     * @brief writeStream 写入数据流
     * @param stream [in] 数据流
     */
    virtual void writeStream(QDataStream &stream) = 0;

    /**
     * @brief saveByteArray 保存为字节组
     * @param byteArray [out] 字节组
     */
    inline void saveByteArray(QByteArray &byteArray)
    {
        QDataStream stream(&byteArray,QIODevice::Append);
        writeStream(stream);
    }
};

/**
 * @brief The IStreamR class
 *      数据流读取接口
 */
class NSB_BLACKBOARD_EXPORT IStreamR
{
public:
    virtual ~IStreamR(){}

    /**
     * @brief readStream 读取数据流
     * @param stream [in] 数据流
     */
    virtual void readStream(QDataStream &stream) = 0;

    /**
     * @brief readByteArray 读取字节组
     * @param byteArray [in] 字节组
     */
    inline void readByteArray(QByteArray &byteArray)
    {
        QDataStream stream(&byteArray,QIODevice::ReadOnly);
        readStream(stream);
    }
};

/**
 * @brief The IStreamWR class
 *      数据流读写接口
 */
class NSB_BLACKBOARD_EXPORT IStreamWR: public IStreamR, public IStreamW
{
};
#endif // ITEMDATAWRITER_H

