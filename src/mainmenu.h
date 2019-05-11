#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include "mainwindow.h"

class MainMenu;

namespace Ui {
class MainMenu;
}

class MainMenu : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenu(QWidget *parent = 0);
    ~MainMenu();

private slots:
    void on_ExitBtn_clicked();

    void on_StartBtn_clicked();

private:
    Ui::MainMenu *ui;
    MainWindow *mainWindow;
};

#endif // MAINMENU_H
