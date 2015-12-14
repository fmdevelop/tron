#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rendering/trongamewindow.h"
#include "data/initobject.h"
#include "data/data.h"
#include <QKeyEvent>
#include <QSharedPointer>

using namespace tron;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_pInitialGameStateGenerator(new gameplay::InitialGameStateGenerator())
  , m_pGame(new gameplay::Game(m_pInitialGameStateGenerator.data()))
{
    ui->setupUi(this);

    connect(this, SIGNAL(gameStart(const tron::data::InitObject&)), m_pGame.data(), SLOT(startGame(const tron::data::InitObject&)));
    connect(m_pGame.data(), SIGNAL(gameFieldInitialized(const data::Field*)), this, SLOT(on_gameFieldInitialized(const data::Field*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // key pressed in this window..
    // TODO should not be necessary, make the TronGameWindow force-focused
    on_keyPressed(event);
}

void MainWindow::on_pb_startGame_clicked()
{
    tron::data::InitObject initObject;
    initObject.fieldSize = QSize(1024, 768),
    initObject.snakeNames[0] = "Master of Desaster";
    initObject.snakeNames[1] = "Evergreen";
    initObject.snakeNames[2] = "Pompino";
    emit gameStart(initObject);
    //m_pGame->startGame(initObject);
}

void MainWindow::on_gameFieldInitialized(const data::Field *field)
{
    // TODO format needed??
    QSurfaceFormat format;
    format.setSamples(16);
    m_pGameWindow.reset(new rendering::TronGameWindow(field));
    // TODO should be an atomar operation: new <-> connect
    connect(m_pGame.data(), SIGNAL(gamestateChanged(QSharedPointer<data::GameState>)),
            m_pGameWindow.data(), SLOT(on_gamestateChanged(QSharedPointer<data::GameState>)));
    connect(m_pGameWindow.data(), SIGNAL(keyPressed(QKeyEvent*)),
            this, SLOT(on_keyPressed(QKeyEvent*)));
    m_pGameWindow->setFormat(format);
    m_pGameWindow->show();
    //m_pGameWindow->setAnimating(true);
}

void MainWindow::on_keyPressed(QKeyEvent *event)
{
    m_pGame->continueWithNextRound();

    switch (event->key()) {
    case Qt::Key_Left:
        m_pGame->changeDirection(0, data::DirectionChange::LEFT);
        break;
    case Qt::Key_Right:
        m_pGame->changeDirection(0, data::DirectionChange::RIGHT);
        break;
    case Qt::Key_Q:
        m_pGame->changeDirection(1, data::DirectionChange::LEFT);
        break;
    case Qt::Key_W:
        m_pGame->changeDirection(1, data::DirectionChange::RIGHT);
        break;
    case Qt::Key_N:
        m_pGame->changeDirection(2, data::DirectionChange::LEFT);
        break;
    case Qt::Key_M:
        m_pGame->changeDirection(2, data::DirectionChange::RIGHT);
        break;

    default:
        event->ignore();
        break;
    }
}
