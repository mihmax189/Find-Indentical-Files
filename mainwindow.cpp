#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  connect(ui->choiceFolderLeft, SIGNAL(clicked()), SLOT(folderExplorerSlot()));
  connect(ui->choiceFolderRight, SIGNAL(clicked()), SLOT(folderExplorerSlot()));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::folderExplorerSlot() {
  QString path;
  // для разных операционных систем будет разная начальная папка
#ifdef Q_OS_LINUX
  path = QFileDialog::getExistingDirectory(
      this, tr("Выбрать папку"), "/home",
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
#elif Q_OS_WIN32
  firstDirectory = QFileDialog::getExistingDirectory(
      this, tr("Выбрать папку"),
      "C:", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
#endif

  if (ui->choiceFolderLeft == qobject_cast<QPushButton *>(sender())) {
    firstDirectory = path;
    ui->showPathToFolderLeft->setText(firstDirectory);
  } else if (ui->choiceFolderRight == qobject_cast<QPushButton *>(sender())) {
    secondDirectory = path;
    ui->showPathToFolderRight->setText(secondDirectory);
  }
}
