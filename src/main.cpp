#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
#include <QDebug>
#include <QtSvg> // Add SVG module header
#include <QDir>
#include <QFile>

#ifdef Q_OS_WIN
// Windows-specific code for GUI application
#include <windows.h>
// No need to declare WinMain manually, Qt will handle this with the WIN32 flag in CMakeLists.txt
#endif

int main(int argc, char *argv[])
{
    // Initialize the application
    QApplication app(argc, argv);
    
    // Debug resource system
    qDebug() << "Checking resource files:";
    QStringList svgPaths = {
        ":/icons/new.svg", 
        ":/icons/open.svg",
        ":/icons/save.svg"
    };
    
    for (const QString &path : svgPaths) {
        QFile file(path);
        qDebug() << path << "exists:" << file.exists();
        if (file.exists()) {
            qDebug() << "  Size:" << file.size() << "bytes";
        }
    }
    
    // Debug resource paths
    qDebug() << "Working directory:" << QDir::currentPath();
    qDebug() << "Resource file check:";
    qDebug() << " - Font Awesome exists:" << QFile::exists(":/fonts/fontawesome-webfont.ttf");
    qDebug() << " - SVG icon exists:" << QFile::exists(":/icons/new.svg");
    
    // List resources directories
    QDir resRoot(":/");
    qDebug() << "Resource root dirs:" << resRoot.entryList(QDir::Dirs);
    
    // Debug resource paths
    qDebug() << "Application directory:" << QDir::currentPath();
    qDebug() << "Resource file exists:" << QFile::exists(":/icons/new.svg");
    
    // List all resources
    qDebug() << "Available resources:";
    QDir resourceDir(":/");
    qDebug() << resourceDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    
    if (resourceDir.exists("icons")) {
        QDir iconDir(":/icons");
        qDebug() << "Icons available:";
        qDebug() << iconDir.entryList(QDir::Files);
    } else {
        qDebug() << "Icons directory not found in resources.";
    }
    
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
