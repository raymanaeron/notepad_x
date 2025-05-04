#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
#include <QDebug>
#include <QtSvg> // Add SVG module header
#include <QDir>
#include <QFile>
#include <QIcon>

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
    
    // Load appropriate app icon based on platform
    QIcon appIcon;
    
#ifdef Q_OS_WIN
    // Windows-specific icon will be loaded from resource,
    // but also try direct file for debugging/fallback
    QString icoPath = QDir(QCoreApplication::applicationDirPath()).filePath("icons/appicons/app_icon_win.ico");
    
    if (QFile::exists(icoPath)) {
        appIcon = QIcon(icoPath);
        qDebug() << "Loaded Windows ICO icon from filesystem:" << icoPath;
    } else {
        qWarning() << "Could not find Windows app icon at:" << icoPath;
        // The icon should still be loaded from the embedded resource
    }
    
    // If no icon was loaded, Windows will use the one from the RC file
#elif defined(Q_OS_MAC)
    // macOS-specific icon - use high-resolution icon for Retina displays
    QString appIconPath = "icons/appicons/notepadx_mac@2x.png";
    QString localPath = QDir(QCoreApplication::applicationDirPath()).filePath(appIconPath);
    if (QFile::exists(localPath)) {
        appIcon = QIcon(localPath);
        qDebug() << "Loaded app icon from filesystem:" << localPath;
    } else {
        qWarning() << "Could not find app icon at:" << localPath;
        
        // Try using working directory as fallback
        localPath = QDir(QDir::currentPath()).filePath(appIconPath);
        if (QFile::exists(localPath)) {
            appIcon = QIcon(localPath);
            qDebug() << "Loaded app icon from current directory:" << localPath;
        }
    }
#else
    // Linux/other platforms icon
    QString appIconPath = "icons/appicons/notepadx_linux.png";
    QString localPath = QDir(QCoreApplication::applicationDirPath()).filePath(appIconPath);
    if (QFile::exists(localPath)) {
        appIcon = QIcon(localPath);
        qDebug() << "Loaded app icon from filesystem:" << localPath;
    } else {
        qWarning() << "Could not find app icon at:" << localPath;
        
        // Try using working directory as fallback
        localPath = QDir(QDir::currentPath()).filePath(appIconPath);
        if (QFile::exists(localPath)) {
            appIcon = QIcon(localPath);
            qDebug() << "Loaded app icon from current directory:" << localPath;
        }
    }
#endif
    
    // If icon was loaded successfully, set it as the application icon
    if (!appIcon.isNull()) {
        QApplication::setWindowIcon(appIcon);
    }
    
#ifdef Q_OS_MAC
    // macOS-specific settings
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
    
    // Enable high DPI support for better icon rendering on Retina displays
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    // For macOS, we don't need to do anything special for toolbar icons
    // They will be managed by the SVG icon provider system
#endif
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
