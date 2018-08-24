#ifndef _WORKER_THREAD_H_
#define _WORKER_THREAD_H_

#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QStringList>
#include <QThread>

#include <QtDebug>

#include "filesinfo.h"

/**
 * Поиск файлов в выбранных папках может занять долгое время, поскольку могут
 * быть вложеные папки. Чтобы ui не замирал, используем отдельный поток для
 * работы.
 * */

class FinderThread : public QThread
{
    Q_OBJECT
    /**
     * Поток для выполнения поиска файлов в выбранной папке.
     * */
  public:
    FinderThread(QObject* parent = nullptr)
      : QThread(parent)
    {
    }
    ~FinderThread() {}
    void run();
    // получить ссылку на директорию, в которой надо произвести поиск
    void setDir(const QDir& dir);
    FilesInfo getFilesInfo();

  private:
    // директоря, в которой надо производить поиск
    QDir directory;
    // список с абсолютными именами найденных файлов
    QStringList absoluteFilesPathesList;
    // иформация о найденных файлах
    QFileInfoList informationFilesList;
    QStringList subDiresList;
    QList<quint64> sizeFiles;

    FilesInfo _filesInfo;

    // поиск файлов
    void find(const QDir& dir);

  signals:
    void end();
};

#endif  //_WORKER_THREAD_H_
