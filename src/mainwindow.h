#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
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
    void on_debug_push_button_clicked();

    void on_activateBtn_clicked();

    void on_deactivateBtn_clicked();

    void timerTimeout();
private:
    Ui::MainWindow *ui;
    GameScene *scene;
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
