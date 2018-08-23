#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <QDebug>
#include <QList>
#include <QSet>
#include <QString>

#include <QByteArray>
#include <QFile>
#include <QThread>

class BinaryFileComparison : public QThread
{
    /**
     * Данный класс реализует бинарное считывание содержимого двух файлов, полные путевые имена которых
     * передаются в его конструктору. Считываение и последующее сравнение содержимого, осуществляется в отдельном
     * потоке.
     * */
    Q_OBJECT

  private:
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
      , error_open(false)
    {
        if (_firstName.isNull() || _firstName.isEmpty() || _secondName.isNull() || _secondName.isEmpty())
            return;

        id = _id;
        firstFileName = _firstName;
        secondFileName = _secondName;
    }

    ~BinaryFileComparison() {}

    void run()
    {
        readFirstFile();
        readSecondFile();
        bool res = compare();

        emit result(firstFileName, secondFileName, id, res);
    }
};

#endif  // _GLOBAL_H_
