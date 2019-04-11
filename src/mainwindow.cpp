#include <QGraphicsView>
#include <QHBoxLayout>
#include <QtDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    repository_(new GameObjectRepository(this)),
    field_(new GameField(this, repository_, 20, 40)),
    textures_(new GameTextureRepository(this)),
    scene(new GameScene(this, repository_, field_, textures_)),
    scheduler(),
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
    
    scheduler.addEvent(new CustomEvent(this), 1000);
    timer.setInterval(40);
    timer.start();
    
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
}

void MainWindow::initObjects()
{
    const char *objects[3] = {"tree1", "tree2", "cinema"};
    for (int i = 0; i < field_->height(); ++i) {
        for (int j = 0; j < field_->width(); ++j) {
            if (qrand() % 12 != 0) {
                continue;
            }
            auto obj = field_->add(new StaticObject(objects[qrand() % 3]));
            obj->setPosition({i, j});
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

void MainWindow::on_listWidget_itemSelectionChanged()
{
    auto items = ui->listWidget->selectedItems();
    if (items.empty()) {
        return;
    }
    auto item = items[0];
    GameObject *object = new StaticObject(item->text());
    
    auto selectionCleaner = [ = ]() {
        item->setSelected(false);
    };
    
    connect(object, &GameObject::placed, this, selectionCleaner);
    connect(object, &GameObject::declined, this, selectionCleaner);
    field_->add(object);
}
