#ifndef TRONGAMEWINDOW_H
#define TRONGAMEWINDOW_H


// TODO proper includes everywhere!!!
#include <QObject>
#include <QOpenGLShaderProgram>
#include "openglwindow.h"
#include "../data/field.h"
#include "../data/border.h"
#include "../data/gamestate.h"
#include "../data/snake.h"
#include <QColor>
#include "text.h"


namespace tron {
namespace rendering {

class TronGameWindow : public OpenGlWindow
{
    Q_OBJECT
public:
    TronGameWindow(const tron::data::Field* field);

    void initialize() Q_DECL_OVERRIDE;
    void render() Q_DECL_OVERRIDE;

public slots:
    void on_gamestateChanged(const data::GameState*newGameState);

private:
    const tron::data::Field* m_pField;
    const tron::data::GameState* m_pGameState;
    tron::rendering::Text m_textRenderer;

    void drawBorder(const tron::data::Border& border);
    void renderSnakeName(const tron::data::Snake& snake);
    void renderLastSegmentOfSnake(const tron::data::Snake& snake);
    void drawCleanField();
};

}
}

#endif // TRONGAMEWINDOW_H
