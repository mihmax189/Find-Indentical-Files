#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <QMap>
#include <QString>
#include <QTime>

#include <limits.h>

#include "binaryfilescompare.h"
#include "filesinfo.h"
#include "workerthread.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

  private:
    Ui::MainWindow* ui;

    // полные путевые имена выбранных директорий
    QString firstDirectory;
    QString secondDirectory;

    // указатели на объекты потоков, которые выполняют поиск файлов в выбранных директориях
    FinderThread* firstFinder;
    FinderThread* secondFinder;

    // объекты, в которых содержится нужная информация по найденным файлам
    FilesInfo firstFilesInfo;
    FilesInfo secondFilesInfo;

    int flag_intersect;
    // данный QMap содержит указатели на объекты BinaryFileCpmarison. В кадом таком объекте
    // выполняется отдельный поток, в котором читается содердимое двух указанных файлов и
    // выполняется последующее бинарное их сравнение. Доступ к конткретному указателю осущствляется
    // по уникальному ключу - id, который генерируется при создании нового экземпляра BinaryFileComparison
    QMap<int, BinaryFileComparison*> processesCompare;

  private slots:
    void freeFirstFinder();
    void freeSecondFinder();
    void folderExplorerSlot();

    void RES(const QString& firstName, const QString& secondName, int id, bool res);

    void compareFiles();

  signals:
    void compare();
};

#endif  // MAINWINDOW_H
