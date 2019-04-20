#include <QGraphicsView>
#include <QHBoxLayout>
#include <QtDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gameobjectpickermodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    repository_(new GameObjectRenderRepository(this)),
    field_(new GameField(this, repository_, 60, 60)),
    textures_(new GameTextureRepository(this)),
    scene(new GameScene(this, repository_, field_, textures_)),
    timer()
{
    textures_->loadFromFile(":/img/textures.json");
    repository_->loadFromFile(":/data/objects.json");
    
    initObjects();
    
    ui->setupUi(this);

    game_view = new GameView(ui->groupBox);
    ui->horizontalLayout->replaceWidget(ui->graphicsView, game_view);
    delete ui->graphicsView;
    ui->graphicsView = game_view;
    
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsView->scale(0.5, 0.5);
    
    field_->scheduler()->addEvent(new CustomEvent(this), 1000, 10);
    timer.setInterval(40);
    timer.start();
    
    GameObjectPickerModel *picker_model = new GameObjectPickerModel(repository_, textures_, this);
    GameObjectSelectionModel *selection_model = new GameObjectSelectionModel(field_, this);
    selection_model->setModel(picker_model);
    ui->listView->setModel(picker_model);
    ui->listView->setSelectionModel(selection_model);
    
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
}

void MainWindow::initObjects()
{
    const char *objects[5] = {"tree1", "tree2", "cinema", "angle-ne", "angle-nw"};
    for (int i = 0; i < field_->height(); ++i) {
        for (int j = 0; j < field_->width(); ++j) {
            if (qrand() % 25 == 0) {
                field_->add("static", objects[qrand() % 5], {i, j});
            }
            if (qrand() % 10 == 0) {
                field_->add("moving", "human", {i, j});
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
    //field_->scheduler()->addEvent(new CustomEvent(this), 10);
    //scheduler.addEvent(new CustomEvent(this), 1000); // devastating :)
}

void MainWindow::on_activateBtn_clicked()
{
    field_->scheduler()->start();
    update();
}

void MainWindow::on_deactivateBtn_clicked()
{
    field_->scheduler()->pause();
    update();
}

void MainWindow::timerTimeout()
{
    field_->scheduler()->update();
    update();
}

void MainWindow::update()
{
    ui->label->setText(QString::asprintf("events caught: %d, active : %s", event_count, field_->scheduler()->active() ? "true" : "false"));
}
