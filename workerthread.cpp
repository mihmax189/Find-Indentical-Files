#include "workerthread.h"

#include <QApplication>
#include <QDebug>

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Методы класса FinderThread
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void FinderThread::setDir(const QDir& dir)
{
    directory = dir;
}

void FinderThread::find(const QDir& dir)
{
    informationFilesList = dir.entryInfoList();

    foreach (QFileInfo file, informationFilesList)
    {
        if (file.isFile())
        {
            absoluteFilesPathesList.append(file.absoluteFilePath());
            sizeFiles.append(file.size());
        }
    }
    // продолжаем поиск файлов в вложенных папках
    QStringList subDires = dir.entryList(QDir::Dirs);

    foreach (QString subdir, subDires)
    {
        if (subdir == "." || subdir == "..")
        {
            continue;
        }

        subDiresList.append(dir.absoluteFilePath(subdir));
        find(QDir(dir.absoluteFilePath(subdir)));
    }
}

void FinderThread::run()
{
    find(directory);
    _filesInfo.initInfo(absoluteFilesPathesList, sizeFiles);
    emit end();
}

void FinderThread::getFilesInfo(FilesInfo& fileInfo)
{
    fileInfo = _filesInfo;
}
