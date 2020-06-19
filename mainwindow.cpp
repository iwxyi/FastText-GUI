#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_lineEdit_editingFinished()
{
    // 命令行示例：https://fasttext.cc/docs/en/cheatsheet.html
}

void MainWindow::on_pushButton_clicked()
{
    FastTextCmd::exec(ui->lineEdit->text().trimmed());
}
