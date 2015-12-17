#include "trongamewindow.h"
#include <QScreen>
#include "../data/data.h"
#include <QFont>
#include <QFontDatabase>

// left, right, top, bottom score screen ratio
#define L_SS_RATIO /7
#define R_SS_RATIO /7*6
#define T_SS_RATIO /5
#define B_SS_RATIO /5*4

const QColor tron::rendering::TronGameWindow::SCORE_SCREEN_BG_COLOR(Qt::darkGray);
const QColor tron::rendering::TronGameWindow::SCORE_SCREEN_FONT_COLOR(Qt::white);


using namespace tron::rendering;

TronGameWindow::TronGameWindow(const data::Field *field)
    : m_pField(field)
    , m_textRenderer(QFontDatabase::systemFont(QFontDatabase::FixedFont))
{
    Q_ASSERT_X(m_pField != NULL, "TronGameWindow", "field NULL");
    QSize fixedSize (m_pField->width, m_pField->height);
    this->resize(fixedSize);
    this->setMinimumSize(fixedSize);
    this->setMaximumSize(fixedSize);
}

void TronGameWindow::drawCleanField()
{
    glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT);

    foreach (const data::Border& border, m_pField->borders) {
        drawBorder(border);
    }
}

void TronGameWindow::initialize()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, width(), height());

    glOrtho(0, width(), height(), 0, 0, 1);

    glMatrixMode(GL_MODELVIEW);

    glClearColor(m_pField->backgroundColor.redF(),
                 m_pField->backgroundColor.greenF(),
                 m_pField->backgroundColor.blueF(),
                 m_pField->backgroundColor.alphaF());
    glClearDepth( 1.0f);

    // TODO...
    glDisable(GL_BLEND);

    drawCleanField();
}

void TronGameWindow::drawExplodedPoints()
{
    QSet<data::Point>::const_iterator it = m_pGameState->explodedPoints.constBegin();
    while (it != m_pGameState->explodedPoints.constEnd()) {
        glVertex2i(it->x, it->y);
        ++it;
    }
}

void TronGameWindow::renderScoreScreenBackground()
{
    glBegin(GL_QUADS);
    glColor3f(SCORE_SCREEN_BG_COLOR.redF(),
              SCORE_SCREEN_BG_COLOR.greenF(),
              SCORE_SCREEN_BG_COLOR.blueF());
    glVertex2i(m_pField->width L_SS_RATIO, m_pField->height T_SS_RATIO);
    glVertex2i(m_pField->width R_SS_RATIO, m_pField->height T_SS_RATIO);
    glVertex2i(m_pField->width R_SS_RATIO, m_pField->height B_SS_RATIO);
    glVertex2i(m_pField->width L_SS_RATIO, m_pField->height B_SS_RATIO);

    glEnd();
}

void TronGameWindow::renderScoreScreenContent(QMap<uint, data::Snake>::const_iterator it)
{
    QMap<QPair<int, QString>, data::Snake> sortingSnakeMap;
    while (it != m_pGameState->snakes.constEnd()) {
        uint score = it->score.getScore();
        sortingSnakeMap.insert(qMakePair(-score, it->name), *it);
        ++it;
    }

    glColor3f(SCORE_SCREEN_FONT_COLOR.redF(),
              SCORE_SCREEN_FONT_COLOR.greenF(),
              SCORE_SCREEN_FONT_COLOR.blueF());

    QList<data::Snake> sortedSnakeList = sortingSnakeMap.values();

    const int lineHeight = 22;

    QString heading;
    QTextStream stream1(&heading);
    stream1 << QString("Name").leftJustified(25);
    stream1.setFieldAlignment(QTextStream::AlignCenter);
    stream1.setFieldWidth(12);
    stream1 << "Collissions"
            << "Kills"
            << "Survivals"
            << "Escapes";
    stream1.setFieldAlignment(QTextStream::AlignRight);
    stream1.setFieldWidth(8);
    stream1 << "Score";

    m_textRenderer.renderText(m_pField->width L_SS_RATIO + lineHeight, m_pField->height T_SS_RATIO + lineHeight, heading, false);

    for (int i = 0; i < sortedSnakeList.count(); ++i) {
        const data::Snake& s = sortedSnakeList[i];
        QString line;
        QTextStream stream2(&line);
        stream2 << s.name.leftJustified(25);
        stream2.setFieldAlignment(QTextStream::AlignCenter);
        stream2.setFieldWidth(12);
        stream2 << s.score.numberOfCollissions
                << s.score.numberOfKills
                << s.score.numberOfSurvivals
                << s.score.numberOfEscapes;
        stream2.setFieldAlignment(QTextStream::AlignRight);
        stream2.setFieldWidth(8);
        stream2 << s.score.getScore();
        m_textRenderer.renderText(m_pField->width L_SS_RATIO + lineHeight, m_pField->height T_SS_RATIO + lineHeight * (i+2), line, false);
    }
}

void TronGameWindow::render()
{
    glMatrixMode(GL_MODELVIEW);

    if (m_pGameState.isNull())
        return;

    QMap<uint, data::Snake>::const_iterator it = m_pGameState->snakes.constBegin();
    switch (m_pGameState->status) {
    case tron::data::ROUND_ABOUT_TO_START:
        drawCleanField();
        while (it != m_pGameState->snakes.constEnd()) {
            renderSnakeName(*it);
            renderLastSegmentOfSnake(*it);
            ++it;
        }
        return;
    case tron::data::RUNNING:
    case tron::data::ROUND_FINISHING:
    case tron::data::ROUND_FINISHED:
        while (it != m_pGameState->snakes.constEnd()) {
            renderLastSegmentOfSnake(*it);
            ++it;
        }


        glBegin(GL_POINTS);
        glColor3f(m_pField->explosionColor.redF(),
                  m_pField->explosionColor.greenF(),
                  m_pField->explosionColor.blueF());

        drawExplodedPoints();
        glEnd();
        return;
    case tron::data::SHOWING_RESULTS:
        renderScoreScreenBackground();

        renderScoreScreenContent(it);

        return;
    }
}

//#include <QDebug>
void TronGameWindow::on_gamestateChanged(QSharedPointer<data::GameState> newGameState)
{
    /*
    static data::Status status = data::ROUND_FINISHED;
    if (status != newGameState->status) {
        status = newGameState->status;
        qDebug() << status;
    }*/

    //qDebug() << "game state changed!!!";
    m_pGameState = newGameState;
    this->requestUpdate();
}

void TronGameWindow::keyPressEvent(QKeyEvent *event)
{
    emit keyPressed(event);
}

void TronGameWindow::drawBorder(const tron::data::Border &border)
{
    glLineWidth(1);
    glColor3f(border.color.redF(),
              border.color.greenF(),
              border.color.blueF());
    glBegin(GL_LINE_STRIP);
    glVertex2i(border.offset, border.offset); // TOP LEFT
    glVertex2i(width() - 1 - border.offset, border.offset); // TOP RIGHT
    glVertex2i(width() - 1 - border.offset, height() - 1 - border.offset); // BOTTOM RIGHT
    glVertex2i(border.offset, height() - 1 - border.offset); // BOTTOM LEFT
    glVertex2i(border.offset, border.offset); // TOP LEFT
    glEnd();
}

void TronGameWindow::renderSnakeName(const tron::data::Snake &snake)
{
    glColor3f(snake.color.redF(),
              snake.color.greenF(),
              snake.color.blueF());
    m_textRenderer.renderText(snake.points.first().x, snake.points.first().y - 20, snake.name);
}

void TronGameWindow::renderLastSegmentOfSnake(const tron::data::Snake &snake)
{
    glLineWidth(1);
    glColor3f(snake.color.redF(),
              snake.color.greenF(),
              snake.color.blueF());
    glBegin(GL_LINE_STRIP);

    glVertex2i(snake.points[snake.points.size()-2].x, snake.points[snake.points.size()-2].y);
    glVertex2i(snake.points.last().x, snake.points.last().y);
    /*
    foreach (const data::Point& p, snake.points) {
        glVertex2i(p.x, p.y);
    }*/
    glEnd();
}
