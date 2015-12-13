#ifndef TRONGAMEWIDGET_H
#define TRONGAMEWIDGET_H

#include "../data/field.h"
#include "../data/gamestate.h"
#include <QObject>
#include <QWidget>
#include <QGLWidget>

class TronGameWidget : public QGLWidget
{
public:
    /*
    TronGameWidget(const tron::data::Field* field);

public slots:
    void on_gamestateChanged(const data::GameState*newGameState);

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;


private:
    const tron::data::Field* m_pField;
    const tron::data::GameState* m_pGameState;

    void drawBorder(const tron::data::Border& border);
    void renderSnakeName(const tron::data::Snake& snake);
    void renderLastSegmentOfSnake(const tron::data::Snake& snake);
    void drawCleanField();
    */
};

#endif // TRONGAMEWIDGET_H
