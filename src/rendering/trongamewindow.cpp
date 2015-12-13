#include "trongamewindow.h"
#include <QScreen>
#include "../data/data.h"

using namespace tron::rendering;

TronGameWindow::TronGameWindow(const data::Field *field)
    : m_pField(field)
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

    drawCleanField();
}

void TronGameWindow::render()
{
    glMatrixMode(GL_MODELVIEW);

    if (m_pGameState == NULL)
        return;

    if (m_pGameState->status == tron::data::ROUND_ABOUT_TO_START) {
        drawCleanField();
        QMap<uint, data::Snake>::const_iterator iterator = m_pGameState->snakes.constBegin();
        while (iterator != m_pGameState->snakes.constEnd()) {
            renderSnakeName(*iterator);
            renderLastSegmentOfSnake(*iterator);
            ++iterator;
        }
        return;
    }

    if (m_pGameState->status == tron::data::RUNNING || m_pGameState->status == tron::data::ROUND_FINISHING) {
        QMap<uint, data::Snake>::const_iterator iterator = m_pGameState->snakes.constBegin();
        while (iterator != m_pGameState->snakes.constEnd()) {
            renderLastSegmentOfSnake(*iterator);
            ++iterator;
        }
        return;
    }

        /*
    glLineWidth(1);
    glColor3f(1.0, 0.0, 0.0);

    glBegin(GL_LINES);
    glVertex2i(0, 0);
    glVertex2i(width(), height());
    glEnd();*/


}

void TronGameWindow::on_gamestateChanged(const tron::data::GameState *newGameState)
{
    m_pGameState = newGameState;
    this->requestUpdate();
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
