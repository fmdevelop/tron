#include "game.h"
#include "../constants.h"
#include <QtGlobal>
#include <QMutexLocker>
#include <QDebug>

using namespace tron::gameplay;


Game::Game(InitialGameStateGenerator *initialGameStateGenerator, QObject *parent) : QObject(parent)
  , m_startGameTimer(new QTimer(this))
  , m_finishingGameTimer(new QTimer(this))
  , m_gameIntervalTimer(new QTimer(this))
  , m_showingScoreTimer(new QTimer(this))
  , m_initialGameStateGenerator(initialGameStateGenerator)
{
    moveToThread(&m_thread);
    m_explosionGenerator.reset(new ExplosionGenerator());
    m_startGameTimer->setSingleShot(true);
    m_finishingGameTimer->setSingleShot(true);
    m_showingScoreTimer->setSingleShot(true);
    m_gameIntervalTimer->setInterval(tron::MSEC_GAME_INTERVAL);
    connect(m_startGameTimer, SIGNAL(timeout()), m_gameIntervalTimer, SLOT(start()));
    connect(m_gameIntervalTimer, SIGNAL(timeout()), this, SLOT(doStep()));
    connect(m_finishingGameTimer, SIGNAL(timeout()), m_gameIntervalTimer, SLOT(stop()));
    connect(m_finishingGameTimer, SIGNAL(timeout()), this, SLOT(roundFinished()));
    connect(m_showingScoreTimer, SIGNAL(timeout()), this, SLOT(showingScoreTimerFinished()));
    m_thread.start();
}

Game::~Game()
{
    m_thread.quit();
    m_thread.wait();
}

void Game::changeDirection(uint id, data::DirectionChange::DirectionChange directionChange)
{
    Q_ASSERT_X(m_gameState != NULL, "changeDirection", "private game state NULL");
    Q_ASSERT_X(m_gameState->snakes.contains(id), "changeDirection", "unknown snake name");

    if (m_gameState->status != tron::data::RUNNING && m_gameState->status != tron::data::ROUND_FINISHING)
        return;

    data::Snake& snake = m_gameState->snakes[id];

    // TODO proper locking... QMutexLocker locker(&snake.mutex());
    QMutexLocker locker(&m_mutexes[id]);
    snake.direction = static_cast<data::Direction::Direction>((snake.direction + directionChange+4)% 4);
}

void Game::continueWithNextRound()
{
    if (m_gameState->status != tron::data::SHOWING_RESULTS)
        return;

    if (m_showingScoreTimer->isActive()) {
        m_waitAfterShowingScore = false;
    } else {
        startRound();
    }
}

void Game::handleSnakeStep(data::Snake& snake)
{
    QMutexLocker locker(&m_mutexes[snake.id]);
    data::Point nextPos = snake.getNextPosition();

    if (nextPos.x < 0 || nextPos.y < 0
            || nextPos.x >= m_field.width
            || nextPos.y >= m_field.height) {
        // ESCAPE
        snake.isAlive = false;
        ++snake.score.numberOfEscapes;
        return;
    }

    int obstacle = (*m_gameMatrix)[nextPos.x][nextPos.y];

    if (obstacle == EMPTY) {
        if (snake.directionHasChanged()) {
            snake.points << nextPos;
        } else {
            snake.points.last() = nextPos;
        }
        m_gameState->explodedPoints.remove(nextPos);
        (*m_gameMatrix)[nextPos.x][nextPos.y] = snake.id;
        return;
    }

    // COLLISSION
    snake.isAlive = false;

    if (obstacle == BORDER) {
        m_explosionGenerator->explode(m_field, nextPos, false, m_roundStarted.elapsed()/1000);
        return; // no points
    }

    // COLLISSION WITH OTHER SNAKE
    uint collissionSnakeId = static_cast<uint>(obstacle);

    bool isFrontalCollision = false;

    if (collissionSnakeId == snake.id) {
       // Collission with himself... --> no points
    } else {
        data::Snake& collissionSnake = m_gameState->snakes[collissionSnakeId];
        isFrontalCollision = collissionSnake.points.last() == nextPos
                && collissionSnake.getNextPosition() == snake.points.last();

        if (isFrontalCollision) {
            // OPTIMIZATION: kill the other snake immediatly and give her points
            // so the handleSnakeStep won't be call for the other snake.
            collissionSnake.isAlive = false;
            ++snake.score.numberOfCollissions;
            ++collissionSnake.score.numberOfCollissions;
        } else {
            if (collissionSnake.isAlive)
                ++collissionSnake.score.numberOfKills;
        }
    }

    m_explosionGenerator->explode(m_field, nextPos, isFrontalCollision, m_roundStarted.elapsed()/1000);
}

void Game::doStep()
{
    if (m_gameState->status == data::ROUND_ABOUT_TO_START) {
        qDebug() << "Changing gamestate to RUNNING in doStep()";
        m_gameState->status = data::RUNNING;
    }

    QMap<uint, data::Snake>::iterator iterator = m_gameState->snakes.begin();

    int snakesAlive = 0;

    QSet<data::Point> snakeHeads;
    while (iterator != m_gameState->snakes.end()) {
        if (!iterator->isAlive) {
            ++iterator;
            continue;
        }

        ++snakesAlive;

        handleSnakeStep(*iterator);

        snakeHeads.insert(iterator->points.last());

        ++iterator;
    }

    if (m_gameState->status == data::RUNNING && snakesAlive <= 1) {
        qDebug() << "Changing gamestate to ROUND_FINISHING in doStep() cause no more snakes are alive";
        m_gameState->status = data::ROUND_FINISHING;

        uint msecToProceed = tron::MSEC_TO_FINISH_ONE_PLAYER_ALIVE;

        if (snakesAlive == 0) {
            msecToProceed = tron::MSEC_TO_FINISH_ALL_PLAYERS_DEAD;
            m_gameIntervalTimer->stop();
        }

        m_finishingGameTimer->start(msecToProceed);
    }


    QSet<data::Point> explodedPoints = m_explosionGenerator->getExplodedPoints();
    QSet<data::Point>::const_iterator it2 = explodedPoints.constBegin();
    while (it2 != explodedPoints.constEnd()) {
        // TODO intersecting the two sets would be more efficient
        if (snakeHeads.contains(*it2)) {
            uint snakeId = static_cast<uint>((*m_gameMatrix)[it2->x][it2->y]);
            m_gameState->snakes[snakeId].isAlive = false;
        }

        (*m_gameMatrix)[it2->x][it2->y] = EMPTY;
        ++it2;
    }

    m_gameState->explodedPoints.unite(explodedPoints);

    fireGameStateChanged();
}

void Game::roundFinished()
{
    QMap<uint, data::Snake>::iterator iterator = m_gameState->snakes.begin();

    while (iterator != m_gameState->snakes.end()) {
        if (iterator->isAlive) {
            ++iterator->score.numberOfSurvivals;
            // there should be only one
            break;
        }
        ++iterator;
    }

    qDebug() << "Changing GAME state to SHOWING RESULTS in roundFinished()";
    m_gameState->status = data::SHOWING_RESULTS;
    fireGameStateChanged();
    m_waitAfterShowingScore = true;
    m_showingScoreTimer->start(tron::MSEC_MIN_SHOWING_SCORE);
}

void Game::startRound()
{
    m_initialGameStateGenerator->cleanupGameStateForNewRound(m_gameState.data(), m_field);
    m_explosionGenerator->clear();

    qDebug() << "Changing game state to ROUND_ABOUT_TO_START in startRound()";
    m_gameState->status = data::ROUND_ABOUT_TO_START;

    fillGameMatrix();

    fireGameStateChanged();

    m_startGameTimer->start(tron::MSEC_TO_START);

    m_roundStarted.start();
}

void Game::showingScoreTimerFinished()
{
    if (!m_waitAfterShowingScore)
        startRound();
}

void Game::fireGameStateChanged()
{
    emit gamestateChanged(m_gameState);
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
            fillGameMatrixColumn(start.x, start.y, end.y, id);
        } else {
            fillGameMatrixRow(start.y, start.x, end.x, id);
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
    uint x1 = qMin(from, to);
    uint x2 = qMax(from, to);
    for (uint x = x1; x <= x2; ++x)
        (*m_gameMatrix)[x][row] = value;
}

void Game::fillGameMatrixColumn(uint column, uint from, uint to, int value)
{
    uint y1 = qMin(from, to);
    uint y2 = qMax(from, to);
    for (uint y = y1; y <= y2; ++y)
        (*m_gameMatrix)[column][y] = value;
}


void Game::startGame(const tron::data::InitObject &initObjectgameState) {
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
