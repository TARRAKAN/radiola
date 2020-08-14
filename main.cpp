#include "player.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));
    Player muzPlayer;
    muzPlayer.show();
    return app.exec();
}
