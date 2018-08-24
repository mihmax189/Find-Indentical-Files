#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <QDebug>
#include <QList>
#include <QSet>
#include <QString>

#include <QByteArray>
#include <QFile>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>

static QWaitCondition countsThreadIsNotFull;
static QMutex mutex;

class BinaryFileComparison : public QThread
{
    /**
     * Данный класс реализует бинарное считывание содержимого двух файлов, полные путевые имена которых
     * передаются в его конструктору. Считываение и последующее сравнение содержимого, осуществляется в отдельном
     * потоке.
     * */
    Q_OBJECT

  private:
    static int currentCountsTread;
    const int COUNTS_THREADS;

    // состояние, когда количество потоков выполняющихся на данный момент меньше COUNT_THREADS
    QString firstFileName;
    QString secondFileName;

    QFile firstFile;
    QFile secondFile;

    QByteArray firstBuffer;
    QByteArray secondBuffer;

    bool error_open;
    int id;

    void readFirstFile()
    {
        firstFile.setFileName(firstFileName);

        if (!firstFile.open(QIODevice::ReadOnly))
        {
            qCritical() << "Error open file: " << firstFileName;
            error_open = true;
            return;
        }

        firstBuffer = firstFile.readAll();
    }

    void readSecondFile()
    {
        secondFile.setFileName(secondFileName);

        if (!secondFile.open(QIODevice::ReadOnly))
        {
            qCritical() << "Error open file: " << secondFileName;
            error_open = true;
            return;
        }

        secondBuffer = secondFile.readAll();
    }

    bool compare()
    {
        /**
         * Сравнивает буферы, в которые содержали содержимое файлов
         * */
        if (error_open)
            return false;

        if (firstBuffer == secondBuffer)
            return true;
        else
            return false;
    }

  signals:
    void result(const QString& first_name, const QString& second_name, int id, bool res);

  public:
    BinaryFileComparison(QString _firstName, QString _secondName, int _id, QObject* parent = nullptr)
      : QThread(parent)
      , COUNTS_THREADS(10)
      , error_open(false)
    {
        if (_firstName.isNull() || _firstName.isEmpty() || _secondName.isNull() || _secondName.isEmpty())
            return;

        id = _id;
        firstFileName = _firstName;
        secondFileName = _secondName;
    }

    ~BinaryFileComparison()
    {
        mutex.lock();
        currentCountsTread--;
        if (currentCountsTread < COUNTS_THREADS)
            countsThreadIsNotFull.wakeAll();
        mutex.unlock();
    }

    void run()
    {
        mutex.lock();
        // ограничиваемся 10 одновременно выполняемыми потоками
        while (currentCountsTread >= COUNTS_THREADS)
            countsThreadIsNotFull.wait(&mutex);
        currentCountsTread++;

        readFirstFile();
        readSecondFile();
        bool res = compare();

        emit result(firstFileName, secondFileName, id, res);
        mutex.unlock();
    }
};

#endif  // _GLOBAL_H_
