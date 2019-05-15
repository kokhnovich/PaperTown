#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <QListWidgetItem>
#include "core/eventscheduler.h"
#include "scene/gamescene.h"
#include "gameindicatorview.h"

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
    void setPause();
    void unsetPause();

private slots:
    void on_startBtn_clicked();

    void on_pauseBtn_clicked();

    void timerTimeout();
    void on_doubleSpinBox_valueChanged(double arg1);

    void on_menuBtn_clicked();

    void on_doubleSpinBox_2_valueChanged(double arg1);

private:
    void initObjects();
    
    Ui::MainWindow *ui;
    GameObjectRenderRepository *repository_;
    GameField *field_;
    GameTextureRepository *textures_;
    GameScene *scene;
    GameIndicatorRepository *indicators_;
    
    QTimer timer;
    int event_count_ = 0;
    double old_scale_ = 0.5;
};

class CustomEvent : public GameEvent
 {
    Q_OBJECT
public:
    CustomEvent(MainWindow *window) : GameEvent(), window_(window) {}

    GameEvent::State activate() override {
        window_->newEvent();
        return GameEvent::Replay;
    }
private:
    MainWindow *window_;
};

#endif // MAINWINDOW_H
