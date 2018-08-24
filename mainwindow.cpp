#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QtDebug>

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , firstDirectory(QString())
  , secondDirectory(QString())
{
    ui->setupUi(this);

    firstFinder = nullptr;
    secondFinder = nullptr;

    flag_intersect = 0;

    connect(ui->scanningButton, SIGNAL(clicked()), SLOT(scanningButtonClicked()));
    connect(ui->choiceFolderLeft, SIGNAL(clicked()), SLOT(folderExplorerSlot()));
    connect(ui->choiceFolderRight, SIGNAL(clicked()), SLOT(folderExplorerSlot()));
    connect(this, SIGNAL(compare()), this, SLOT(compareFiles()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::folderExplorerSlot()
{
    QString path;
    // для разных операционных систем будет разная начальная папка
#ifdef Q_OS_LINUX
    path = QFileDialog::getExistingDirectory(this, tr("Выбрать папку"), "/home",
                                             QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
#elif Q_OS_WIN32
    firstDirectory = QFileDialog::getExistingDirectory(
            this, tr("Выбрать папку"), "C:", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
#endif
    if (path.isEmpty() || path.isNull())
    {
        ui->textEdit->append("Папка не выбрана");
        return;
    }

    if (ui->choiceFolderLeft == qobject_cast<QPushButton*>(sender()))
    {
        firstDirectory = path;
        ui->showPathToFolderLeft->setText(firstDirectory);
        firstFinder = new FinderThread(this);

        firstFinder->setDir(QDir(firstDirectory));
    }
    else if (ui->choiceFolderRight == qobject_cast<QPushButton*>(sender()))
    {
        secondDirectory = path;
        ui->showPathToFolderRight->setText(secondDirectory);
        secondFinder = new FinderThread(this);
        secondFinder->setDir(QDir(secondDirectory));
    }
}

void MainWindow::freeFirstFinder()
{
    // данный слот вызывается по завершении работы первого потока, который ищет все файлы, в первой выбранной папке
    firstFilesInfo = firstFinder->getFilesInfo();
    QList<QString> keys = firstFilesInfo.getSizes();

    if (++flag_intersect == 2)
        emit compare();

    qDebug() << "freeFirstFinder";
    delete firstFinder;
}

void MainWindow::freeSecondFinder()
{
    // данный слот вызывается по завершении работы второго потока, который ищет все файлы, во второй выбранной папке
    secondFilesInfo = secondFinder->getFilesInfo();
    QList<QString> keys = secondFilesInfo.getSizes();

    if (++flag_intersect == 2)
        emit compare();

    qDebug() << "freeSecondFinder";
    delete secondFinder;
}

void MainWindow::compareFiles()
{
    qDebug() << "compareFiles";
    // если указанные папки пустые - выходим
    if (firstFilesInfo.getNames().isEmpty() || secondFilesInfo.getNames().isEmpty())
        return;

    // делаем пересечение множеств ключей (размеров файлов) из двух папок. в результате должны получить
    // список, в котором будут находиться размеры, которые есть и в одной и другой папках.
    QList<QString> keys = intersectKeys(firstFilesInfo.getSizes(), secondFilesInfo.getSizes());
    flag_intersect = 0;
    int id;

    if (keys.isEmpty())
        return;

    foreach (auto it, keys)
    {
        QList<QString> t1 = firstFilesInfo.getName(it);
        QList<QString> t2 = secondFilesInfo.getName(it);

        for (int file_1 = 0; file_1 < t1.size(); ++file_1)
        {
            for (int file_2 = 0; file_2 < t2.size(); ++file_2)
            {
                // генерируем id потока, который будет считывать данные файлов и сравнивать их в бинарном виде,
                // чтобы потом, корректно удалить его
                id = qrand() % INT_MAX;

                processesCompare.insert(id, new BinaryFileComparison(t1[file_1], t2[file_2], id, this));
                connect(processesCompare[id], SIGNAL(result(const QString&, const QString&, int, bool)), this,
                        SLOT(RES(const QString&, const QString&, int, bool)));
                processesCompare[id]->start();
            }
        }
    }
}

void MainWindow::RES(const QString& firstName, const QString& secondName, int id, bool res)
{
    if (res)
    {
        ui->textEdit->append("Files are the same:");
        ui->textEdit->append(firstName);
        ui->textEdit->append(secondName);
    }

    delete processesCompare[id];
    processesCompare.remove(id);

    if (processesCompare.isEmpty())
    {
        // очищаем все исходные данные по выполнению работы, для следующего запуска поиска идентичных файлов
        qDebug() << "DEBUG: processesCompare is empty!";
        firstFilesInfo = FilesInfo();
        secondFilesInfo = FilesInfo();

        firstFinder = nullptr;
        secondFinder = nullptr;

        firstDirectory = QString();
        secondDirectory = QString();
    }
}

void MainWindow::scanningButtonClicked()
{
    if (firstFinder == nullptr || secondFinder == nullptr || firstDirectory.isEmpty() || firstDirectory.isNull() ||
        secondDirectory.isEmpty() || secondDirectory.isNull())
        return;

    connect(firstFinder, SIGNAL(end()), this, SLOT(freeFirstFinder()));
    connect(secondFinder, SIGNAL(end()), this, SLOT(freeSecondFinder()));

    firstFinder->start();
    secondFinder->start();
    ui->textEdit->append("scannnnnning!");
}
