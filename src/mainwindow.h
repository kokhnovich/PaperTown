#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <QListWidgetItem>
#include "core/eventscheduler.h"
#include "scene/gamescene.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void newEvent();
    void update();
private slots:
    void on_activateBtn_clicked();

    void on_deactivateBtn_clicked();

    void timerTimeout();

    void on_listWidget_itemSelectionChanged();

private:
    void initObjects();
    
    Ui::MainWindow *ui;
    
    GameObjectRepository *repository_;
    GameField *field_;
    GameTextureRepository *textures_;
    GameScene *scene;
    GameView *game_view;
    
    GameEventScheduler scheduler;
    QTimer timer;
    int event_count = 0;
};

class CustomEvent : public GameAbstractEvent {
    Q_OBJECT
public:
    CustomEvent(MainWindow *window) : GameAbstractEvent(), window_(window) {}

    void activate() override {
        window_->newEvent();
    }
private:
    MainWindow *window_;
};

#endif // MAINWINDOW_H
