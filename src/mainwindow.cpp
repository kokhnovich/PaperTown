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

    scene->setSceneRect(-500, -500, 500, 500);

    QBrush brush(QPixmap(":/img/cell.png"));
    scene->setBackgroundBrush(brush);

    ui->graphicsView->setScene(scene);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

}

MainWindow::~MainWindow()
{
    delete ui;
}
