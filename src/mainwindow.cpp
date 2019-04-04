#include <QGraphicsView>
#include <QHBoxLayout>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    repository_(new GameObjectRepository(this)),
    field_(new GameField(this, repository_, 42, 80)),
    textures_(new GameTextureRepository(this)),
    scene(new GameScene(this, repository_, field_, textures_)),
    scheduler(),
    timer()
{
    textures_->loadFromFile(":/img/textures.json");
    repository_->loadFromFile(":/data/objects.json");
    
    initObjects();
    
    ui->setupUi(this);

    ui->graphicsView->setScene(scene);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsView->scale(0.5, 0.5);
    
    scheduler.addEvent(new CustomEvent(this), 1000);
    timer.setInterval(40);
    timer.start();

    connect(&timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
}

void MainWindow::initObjects()
{
    const char *objects[3] = {"tree1", "tree2", "cinema"};
    for (int i = 0; i < field_->height(); i += 2) {
        for (int j = 0; j < field_->width(); j += 2) {
            if (qrand() % 3 != 0) {
                continue;
            }
            auto obj = field_->add(new StaticObject(objects[qrand() % 3]));
            if (!obj->setPosition({i, j})) {
                field_->remove(obj);
            }
        }
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newEvent()
{
    //auto objects = scene->field()->getByType("static")->get();
    //scene->field()->remove(objects[qrand() % objects.size()]);
    
    ++event_count;
    scheduler.addEvent(new CustomEvent(this), 1000);
    //scheduler.addEvent(new CustomEvent(this), 1000); // devastating :)
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
