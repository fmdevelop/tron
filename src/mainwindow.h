#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>
#include "rendering/trongamewindow.h"
#include "gameplay/game.h"
#include "gameplay/initialgamestategenerator.h"
#include "data/field.h"

namespace Ui {
class MainWindow;
}

namespace tron {

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void gameStart(const tron::data::InitObject&);

protected:
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private slots:
    void on_pb_startGame_clicked();
    void on_gameFieldInitialized(const data::Field* field);
    void on_keyPressed(QKeyEvent *event);

private:
    Ui::MainWindow *ui;

    QScopedPointer<gameplay::InitialGameStateGenerator> m_pInitialGameStateGenerator;
    QScopedPointer<gameplay::Game> m_pGame;
    QScopedPointer<rendering::TronGameWindow> m_pGameWindow;
};
}

#endif // MAINWINDOW_H
