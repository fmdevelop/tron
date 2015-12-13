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

private slots:
    void on_pb_startGame_clicked();
    void on_gameFieldInitialized(const data::Field* field);

private:
    Ui::MainWindow *ui;

    QScopedPointer<gameplay::InitialGameStateGenerator> m_pInitialGameStateGenerator;
    gameplay::Game* m_pGame;
    QScopedPointer<rendering::TronGameWindow> m_pGameWindow;
};
}

#endif // MAINWINDOW_H
