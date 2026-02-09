#include "mainWindow.hpp"
#include <QScreen>
#include <qt6/QtWidgets/QApplication>

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    app.setDesktopFileName("wlspotifyOverlay");
    MainWindow window;
    window.show();

    return app.exec();
}
