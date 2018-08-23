#ifndef _FILEINFO_H_
#define _FILEINFO_H_

#include <QList>
#include <QMultiHash>
#include <QSet>
#include <QStringList>

#include <QDebug>

class FilesInfo
{
    /**
     * Данный класс содержит информацию, по найденным файлам. В частности: полные путевые имена
     * файлов и их размеры.
     * **/
  public:
    FilesInfo() {}

    void initInfo(const QStringList& absFileNames, const QList<quint64> fileSizes)
    {
        // проверяем правильность входных аргументов
        if ((absFileNames.size() != fileSizes.size()) || absFileNames.isEmpty() || fileSizes.isEmpty())
            return;

        auto it_size = fileSizes.begin();
        auto it_names = absFileNames.begin();

        for (; it_names != absFileNames.end() && it_size != fileSizes.end(); ++it_names, ++it_size)
            files.insert(QString::number(*it_size), *it_names);
    }

    QList<QString> getSizes()
    {
        // получить все ключи - размеры файлов - QMultiHash<> files;
        return files.keys();
    }

    QList<QString> getNames()
    {
        return files.values();
    }

    QString getName(const QString& key)
    {
        auto name = files.find(key);
        return name.value();
    }

  private:
    QMultiHash<QString, QString> files;
};

#endif  // _FILEINFO_H_
