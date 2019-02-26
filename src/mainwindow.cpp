#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsView>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);

    QBrush brush(QPixmap("/cell3.png"));
    scene->setBackgroundBrush(brush);

    ui->graphicsView->setScene(scene);
}

MainWindow::~MainWindow()
{
    delete ui;
}
