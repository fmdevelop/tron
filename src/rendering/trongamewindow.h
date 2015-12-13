#ifndef TRONGAMEWINDOW_H
#define TRONGAMEWINDOW_H

#include <QObject>
#include <QOpenGLShaderProgram>
#include "openglwindow.h"
#include "../data/field.h"
#include "../data/border.h"
#include "../data/gamestate.h"
#include "../data/snake.h"
#include <QColor>


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

    void drawBorder(const tron::data::Border& border);
    void renderSnakeName(const tron::data::Snake& snake);
    void renderSnake(const tron::data::Snake& snake);
    void drawCleanField();
};

}
}

#endif // TRONGAMEWINDOW_H
