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
    field_(new GameField(this, repository_, 80, 80)),
    textures_(new GameTextureRepository(this)),
    scene(new GameScene(this, repository_, field_, textures_)),
    player_(nullptr),
    timer()
{
    textures_->loadFromFile(":/img/textures.json");
    repository_->loadFromFile(":/data/objects.json");
    
    //auto human = field_->add("moving", "human", {field_->height() / 2, field_->width() / 2});
    //player_ = gameProperty_cast<GameProperty_human>(human->property());
    initObjects();
    
    ui->setupUi(this);

    ui->graphicsView->setScene(scene);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsView->scale(0.5, 0.5);
    old_scale_ = 0.5;
    
    field_->scheduler()->addEvent(new CustomEvent(this), 1000, 10);
    timer.setInterval(40);
    timer.start();
    
    GameObjectPickerModel *picker_model = new GameObjectPickerModel(repository_, textures_, this);
    GameObjectSelectionModel *selection_model = new GameObjectSelectionModel(field_, this);
    selection_model->setModel(picker_model);
    ui->listView->setModel(picker_model);
    ui->listView->setSelectionModel(selection_model);
    
    //this->grabKeyboard();
    
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);
    if (player_ == nullptr) {
        return;
    }
    if (event->key() == Qt::Key_Up) {
        player_->setDirection(Util::Up);
    }
    if (event->key() == Qt::Key_Down) {
        player_->setDirection(Util::Down);
    }
    if (event->key() == Qt::Key_Left) {
        player_->setDirection(Util::Left);
    }
    if (event->key() == Qt::Key_Right) {
        player_->setDirection(Util::Right);
    }
    if (event->key() == Qt::Key_Space) {
        if (player_->isActive()) {
            player_->stop();
        } else {
            player_->go();
        }
    }
}

void MainWindow::initObjects()
{
    const char *objects[5] = {"tree1", "tree2", "cinema", "angle-ne", "angle-nw"};
    for (int i = 0; i < field_->height(); ++i) {
        for (int j = 0; j < field_->width(); ++j) {
            /*if (qrand() % 25 == 0) {
                field_->add("static", objects[qrand() % 5], {i, j});
            }*/
            if (qrand() % 25 == 0) {
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
    ++event_count_;
}

void MainWindow::on_startBtn_clicked()
{
    field_->scheduler()->start();
    update();
}

void MainWindow::on_pauseBtn_clicked()
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
    ui->label->setText(QString::asprintf("events caught: %d, active : %s", event_count_, field_->scheduler()->active() ? "true" : "false"));
    ui->startBtn->setEnabled(!field_->scheduler()->active());
    ui->pauseBtn->setEnabled(field_->scheduler()->active());
}

void MainWindow::on_doubleSpinBox_valueChanged(double arg1)
{
    double scale = arg1 / 100.0;
    ui->graphicsView->scale(scale/old_scale_, scale/old_scale_);
    old_scale_ = scale;
}
