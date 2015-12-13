#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rendering/trongamewindow.h"
#include "data/initobject.h"

using namespace tron;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_pInitialGameStateGenerator(new gameplay::InitialGameStateGenerator())
  , m_pGame(new gameplay::Game(m_pInitialGameStateGenerator.data(), this))
{
    ui->setupUi(this);

    connect(m_pGame, SIGNAL(gameFieldInitialized(const data::Field*)), this, SLOT(on_gameFieldInitialized(const data::Field*)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb_startGame_clicked()
{
    tron::data::InitObject initObject;
    initObject.fieldSize = QSize(640, 480),
    initObject.snakeNames << "Master of Desaster" << "Evergreen" << "Pompino";
    m_pGame->startGame(initObject);
}

void MainWindow::on_gameFieldInitialized(const data::Field *field)
{
    // TODO format needed??
    QSurfaceFormat format;
    format.setSamples(16);
    m_pGameWindow.reset(new rendering::TronGameWindow(field));
    // TODO should be an atomar operation: new <-> connect
    connect(m_pGame, SIGNAL(gamestateChanged(const data::GameState*)),
            m_pGameWindow.data(), SLOT(on_gamestateChanged(const data::GameState*)));
    m_pGameWindow->setFormat(format);
    m_pGameWindow->show();
    //m_pGameWindow->setAnimating(true);
}
