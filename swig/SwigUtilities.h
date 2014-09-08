#ifndef _SWIG_UTILITIES_H_
#define _SWIG_UTILITIES_H_

#include <OsmAndCore/stdlib_common.h>

#include <OsmAndCore/QtExtensions.h>
#include <QString>
#include <QByteArray>
#include <QFile>

namespace OsmAnd
{
    struct SwigUtilities
    {
        inline static QByteArray readEntireFile(const QString& filename)
        {
            QFile file(filename);
            if (!file.open(QIODevice::ReadOnly))
                return QByteArray();

            const auto data = file.readAll();

            file.close();

            return data;
        }

        inline static QByteArray readPartOfFile(const QString& filename, const size_t offset, const size_t length)
        {
            QFile file(filename);
            if (!file.open(QIODevice::ReadOnly))
                return QByteArray();

            if (!file.seek(offset))
                return QByteArray();

            QByteArray data;
            data.resize(length);

            auto pData = data.data();
            size_t totalBytesRead = 0;
            while (totalBytesRead < length)
            {
                const auto bytesRead = file.read(pData, length - totalBytesRead);
                if (bytesRead < 0)
                    break;
                pData += bytesRead;
                totalBytesRead += static_cast<size_t>(bytesRead);
            }
            file.close();

            if (totalBytesRead != length)
                return QByteArray();
            return data;
        }

        inline static QByteArray emptyQByteArray()
        {
            return QByteArray();
        }

    private:
        SwigUtilities();
        ~SwigUtilities();
    };
}

#endif // !defined(_SWIG_UTILITIES_H_)
