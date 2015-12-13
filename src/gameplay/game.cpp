#include "game.h"
#include "../constants.h"
#include <QtGlobal>
#include <QMutexLocker>

using namespace tron::gameplay;


Game::Game(InitialGameStateGenerator *initialGameStateGenerator, QObject *parent) : QObject(parent)
  , m_startGameTimer(new QTimer(this))
  , m_finishingGameTimer(new QTimer(this))
  , m_gameIntervalTimer(new QTimer(this))
  , m_initialGameStateGenerator(initialGameStateGenerator)
{
    m_gameIntervalTimer->setInterval(tron::MSEC_GAME_INTERVAL);
    connect(m_startGameTimer, SIGNAL(timeout()), m_gameIntervalTimer, SLOT(start()));
    connect(m_gameIntervalTimer, SIGNAL(timeout()), this, SLOT(doStep()));
    connect(m_finishingGameTimer, SIGNAL(timeout()), m_gameIntervalTimer, SLOT(stop()));
    connect(m_finishingGameTimer, SIGNAL(timeout()), this, SLOT(roundFinished()));
}

void Game::changeDirection(uint id, data::DirectionChange::DirectionChange directionChange)
{
    Q_ASSERT_X(m_gameState != NULL, "changeDirection", "private game state NULL");
    Q_ASSERT_X(m_gameState->snakes.contains(id), "changeDirection", "unknown snake name");

    data::Snake& snake = m_gameState->snakes[id];

    // TODO proper locking... QMutexLocker locker(&snake.mutex());
    snake.direction = static_cast<data::Direction::Direction>((snake.direction + directionChange) % 4);
}

void Game::handleSnakeStep(data::Snake& snake)
{
    data::Point nextPos = snake.getNextPosition();

    if (nextPos.x < 0 || nextPos.y < 0
            || nextPos.x >= m_field.width
            || nextPos.y >= m_field.height) {
        // ESCAPE
        snake.isAlive = false;
        snake.score += SCORE_ESCAPE;
        return;
    }

    int obstacle = (*m_gameMatrix)[nextPos.x][nextPos.y];

    if (obstacle == EMPTY) {
        if (snake.directionHasChanged()) {
            snake.points << nextPos;
        } else {
            snake.points.last() = nextPos;
        }
        (*m_gameMatrix)[nextPos.x][nextPos.y] = snake.id;
        return;
    }

    // COLLISSION
    snake.isAlive = false;

    if (obstacle == BORDER)
        return; // no points

    // COLLISSION WITH OTHER SNAKE
    uint collissionSnakeId = static_cast<uint>(obstacle);
    if (collissionSnakeId == snake.id) {
       // no points for user --> OR -1 ?
       return;
    }
    data::Snake& collissionSnake = m_gameState->snakes[collissionSnakeId];
    bool isFrontalCollision = collissionSnake.points.last() != nextPos;

    if (isFrontalCollision) {
        collissionSnake.isAlive = false;
        collissionSnake.score += SCORE_COLLISSION;
        snake.score += SCORE_COLLISSION;
    } else {
        collissionSnake.score += SCORE_KILL;
    }
}

void Game::doStep()
{
    if (m_gameState->status == data::ROUND_ABOUT_TO_START)
        m_gameState->status = data::RUNNING;

    QMap<uint, data::Snake>::iterator iterator = m_gameState->snakes.begin();

    int snakesAlive = 0;

    while (iterator != m_gameState->snakes.end()) {
        if (!iterator->isAlive) {
            ++iterator;
            continue;
        }

        ++snakesAlive;

        handleSnakeStep(*iterator);

        ++iterator;
    }

    if (m_gameState->status == data::RUNNING && snakesAlive <= 1) {
        m_gameState->status = data::ROUND_FINISHING;

        uint msecToProceed = tron::MSEC_TO_FINISH_ONE_PLAYER_ALIVE;

        if (snakesAlive == 0) {
            msecToProceed = tron::MSEC_TO_FINISH_ALL_PLAYERS_DEAD;
            m_gameIntervalTimer->stop();
        }

        m_finishingGameTimer->start(msecToProceed);
    }

    fireGameStateChanged();
}

void Game::roundFinished()
{
    QMap<uint, data::Snake>::iterator iterator = m_gameState->snakes.begin();

    while (iterator != m_gameState->snakes.end()) {
        if (iterator->isAlive) {
            iterator->score += SCORE_SURVIVAL;
            // there should be only one
            break;
        }
        ++iterator;
    }

    m_gameState->status = data::SHOWING_RESULTS;
    fireGameStateChanged();
}

void Game::startRound()
{
    m_initialGameStateGenerator->cleanupGameStateForNewRound(m_gameState.data(), m_field);

    fillGameMatrix();

    fireGameStateChanged();

    m_startGameTimer->start(tron::MSEC_TO_START);
}

void Game::fireGameStateChanged()
{
    emit gamestateChanged(m_gameState.data());
}

void Game::fillGameMatrix()
{
    data::Field f = m_field;

    for (int x = 0; x < f.width; ++x)
        for (int y = 0; y < f.height; ++y)
            (*m_gameMatrix)[x][y] = EMPTY;

    QMap<uint, data::Snake>::const_iterator iterator = m_gameState->snakes.constBegin();

    while (iterator != m_gameState->snakes.constEnd()) {
        uint id = iterator.key();
        data::Snake snake = iterator.value();

        Q_ASSERT_X(snake.points.count() >= 2, "fillGameMatrix", "Invalid game state to start a game");

        data::Point start = snake.points[snake.points.count() - 2];
        data::Point end = snake.points.last();

        Q_ASSERT_X(start.x == end.x || start.y == end.y, "fillGameMatrix", "Invalid start points for a snake");

        if (start.x == end.x) {
            fillGameMatrixRow(start.x, start.y, end.y, id);
        } else {
            fillGameMatrixColumn(start.y, start.x, end.x, id);
        }

        ++iterator;
    }

    foreach(const data::Border& b, f.borders) {
        fillGameMatrixRow(b.offset, b.offset, f.width - 1 - b.offset, BORDER);
        fillGameMatrixRow(f.height - 1 - b.offset, b.offset, f.width - 1 - b.offset, BORDER);
        fillGameMatrixColumn(b.offset, b.offset, f.height -1 - b.offset, BORDER);
        fillGameMatrixColumn(f.width - 1 - b.offset, b.offset, f.height -1 - b.offset, BORDER);
    }
}

void Game::fillGameMatrixRow(uint row, uint from, uint to, int value)
{
    for (uint x = from; x <= to; ++x)
        (*m_gameMatrix)[x][row] = value;
}

void Game::fillGameMatrixColumn(uint column, uint from, uint to, int value)
{
    for (uint y = from; y <= to; ++y)
        (*m_gameMatrix)[column][y] = value;
}


void Game::startGame(const data::InitObject &initObjectgameState) {
    Q_ASSERT_X(initObjectgameState.snakeNames.count() > 0, "startGame", "No snakes to start a game");
    Q_ASSERT_X(initObjectgameState.fieldSize.width() > 0, "startGame", "Invalid field width");
    Q_ASSERT_X(initObjectgameState.fieldSize.height() > 0, "startGame", "Invalid field height");

    m_field = m_initialGameStateGenerator->createGameField(initObjectgameState.fieldSize);
    data::GameState* gameState = m_initialGameStateGenerator->createInitialGameState(initObjectgameState);
    m_gameState.reset(gameState);

    // initialize GameMatrix
    // vector of columns
    QVector<QVector<int> >* vec = new QVector<QVector<int> >(m_field.width);
    for (int x = 0; x < vec->length(); ++x)
        (*vec)[x].resize(m_field.height);
    m_gameMatrix.reset(vec);

    emit gameFieldInitialized(&m_field);

    startRound();
}
