#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsView>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    scheduler(),
    timer()
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);

    scene->setSceneRect(-500, -500, 500, 500);

    QBrush brush(QPixmap(":/img/cell.png"));
    scene->setBackgroundBrush(brush);

    ui->graphicsView->setScene(scene);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    scheduler.addEvent(new CustomEvent(this), 1000);
    timer.setInterval(40);
    timer.start();

    connect(&timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newEvent()
{
    ++event_count;
    scheduler.addEvent(new CustomEvent(this), 1000);
}

void MainWindow::on_debug_push_button_clicked()
{
    ui->debug_label->setText("default");
}

void MainWindow::on_activateBtn_clicked()
{
    scheduler.start();
    update();
}

void MainWindow::on_deactivateBtn_clicked()
{
    scheduler.pause();
    update();
}

void MainWindow::timerTimeout()
{
    scheduler.update();
    update();
}

void MainWindow::update()
{
    ui->label->setText(QString::asprintf("events caught: %d, active : %s", event_count, scheduler.active() ? "true" : "false"));
}
