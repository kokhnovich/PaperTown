#include <QFileDialog>
#include <iostream>
#include "mainmenu.h"
#include "ui_mainmenu.h"
#include "mainwindow.h"

MainMenu::MainMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainMenu),
    mainWindow(new MainWindow(this))
{
    mainWindow->setPause();
    ui->setupUi(this);
}

MainMenu::~MainMenu()
{
    delete ui;
}

void MainMenu::on_ExitBtn_clicked()
{
    mainWindow->close();
    this->close();
}

void MainMenu::on_StartBtn_clicked()
{
    mainWindow->show();
    mainWindow->unsetPause();
}

void MainMenu::on_chooseMusicBtn_clicked()
{
    // TODO user can select music file
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    //std::cout << fileName << std::endl;
}
