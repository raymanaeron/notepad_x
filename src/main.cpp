#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
#include <QDebug>

#ifdef Q_OS_WIN
// On Windows, set the application to be a GUI application
// without a console window
#include <windows.h>
extern "C" {
    // This sets the application type to Windows GUI
    // 1 = GUI, 2 = Console
    int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
}
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Load FontAwesome font
    int fontId = QFontDatabase::addApplicationFont(":/fonts/fontawesome-webfont.ttf");
    if (fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty()) {
            // The font is available as fontFamilies.at(0)
            qDebug() << "FontAwesome loaded:" << fontFamilies.at(0);
        }
    } else {
        qWarning() << "Failed to load FontAwesome";
    }
    
    MainWindow window;
    window.show();
    return app.exec();
}
