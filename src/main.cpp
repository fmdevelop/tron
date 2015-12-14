#include "mainwindow.h"
#include <QApplication>
#include <QMetaType>
#include "data/initobject.h"

using namespace tron;

Q_DECLARE_METATYPE(QSharedPointer<data::GameState>)

int main(int argc, char *argv[])
{

    qRegisterMetaType<tron::data::InitObject>();
    qRegisterMetaType<QSharedPointer<data::GameState> >();



    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
