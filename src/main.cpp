#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
#include <QDebug>
#include <QtSvg> // Add SVG module header

#ifdef Q_OS_WIN
// Windows-specific code for GUI application
#include <windows.h>
// No need to declare WinMain manually, Qt will handle this with the WIN32 flag in CMakeLists.txt
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Load FontAwesome font
    int fontId = QFontDatabase::addApplicationFont(":/fonts/fontawesome-webfont.ttf");
    if (fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty()) {
            qDebug() << "FontAwesome loaded:" << fontFamilies.at(0);
        }
    } else {
        qWarning() << "Failed to load FontAwesome";
    }
    
    // Set application metadata
    QApplication::setApplicationName("Notepad X");
    QApplication::setOrganizationName("Notepad X");
    QApplication::setApplicationVersion("1.0.0");
    
#ifdef Q_OS_MAC
    // macOS-specific settings
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
#endif
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
