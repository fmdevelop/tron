#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rendering/openglwindow.h"

using namespace tron;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb_startGame_clicked()
{
    rendering::OpenGlWindow* window = new rendering::OpenGlWindow();
    window->resize(640, 480);
    window->show();
    window->setAnimating(true);
}
