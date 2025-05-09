#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
#include <QDebug>
#include <QtSvg> // Add SVG module header
#include <QDir>
#include <QFile>
#include <QIcon>
#include <QMessageBox>
#include <QFileInfo>
#include <iostream>

// Debug helper function
void showDebugMessage(const QString& message) {
    qDebug() << message;
    std::cout << message.toStdString() << std::endl;
    QFile file("crash_log.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream stream(&file);
        stream << message << "\n";
        file.close();
    }
}

#ifdef Q_OS_WIN
// Windows-specific code for GUI application
#include <windows.h>
// No need to declare WinMain manually, Qt will handle this with the WIN32 flag in CMakeLists.txt
#endif

int main(int argc, char *argv[])
{
    // Create an empty crash log file
    QFile crashFile("crash_log.txt");
    if (crashFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream stream(&crashFile);
        stream << "Starting application...\n";
        stream << "Current directory: " << QDir::currentPath() << "\n";
        crashFile.close();
    }

    try {
        showDebugMessage("Initializing application");
        
        // Initialize the application
        QApplication app(argc, argv);
        
        showDebugMessage("Application initialized");
        
        // Debug resource system
        showDebugMessage("Checking resource files");
        QStringList svgPaths = {
            ":/icons/new.svg", 
            ":/icons/open.svg",
            ":/icons/save.svg"
        };
        
        for (const QString &path : svgPaths) {
            QFile file(path);
            showDebugMessage(QString("%1 exists: %2").arg(path).arg(file.exists()));
            if (file.exists()) {
                showDebugMessage(QString("  Size: %1 bytes").arg(file.size()));
            }
        }
        
        // Debug resource paths
        showDebugMessage(QString("Working directory: %1").arg(QDir::currentPath()));
        showDebugMessage(QString("Resource file check: Font Awesome exists: %1").arg(QFile::exists(":/fonts/fontawesome-webfont.ttf")));
        showDebugMessage(QString("Resource file check: SVG icon exists: %1").arg(QFile::exists(":/icons/new.svg")));
        
        // List resources directories
        QDir resRoot(":/");
        showDebugMessage(QString("Resource root dirs: %1").arg(resRoot.entryList(QDir::Dirs).join(", ")));
        
        // Debug resource paths
        showDebugMessage(QString("Application directory: %1").arg(QDir::currentPath()));
        showDebugMessage(QString("Resource file exists: %1").arg(QFile::exists(":/icons/new.svg")));
        
        // List all resources
        showDebugMessage("Available resources:");
        QDir resourceDir(":/");
        showDebugMessage(resourceDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot).join(", "));
        
        if (resourceDir.exists("icons")) {
            QDir iconDir(":/icons");
            showDebugMessage("Icons available:");
            showDebugMessage(iconDir.entryList(QDir::Files).join(", "));
        } else {
            showDebugMessage("Icons directory not found in resources.");
        }
        
        // Load FontAwesome font
        int fontId = QFontDatabase::addApplicationFont(":/fonts/fontawesome-webfont.ttf");
        if (fontId != -1) {
            QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
            if (!fontFamilies.isEmpty()) {
                showDebugMessage(QString("FontAwesome loaded: %1").arg(fontFamilies.at(0)));
            }
        } else {
            showDebugMessage("Failed to load FontAwesome");
        }
        
        // Set application metadata
        QApplication::setApplicationName("NotepadX");
        QApplication::setOrganizationName("NotepadX");
        QApplication::setApplicationVersion("1.0.0");
        
        // Load appropriate app icon based on platform
        QIcon appIcon;
    
#ifdef Q_OS_WIN
        // Windows-specific icon will be loaded from resource,
        // but also try direct file for debugging/fallback
        QString icoPath = QDir(QCoreApplication::applicationDirPath()).filePath("icons/appicons/app_icon_win.ico");
        
        if (QFile::exists(icoPath)) {
            appIcon = QIcon(icoPath);
            showDebugMessage(QString("Loaded Windows ICO icon from filesystem: %1").arg(icoPath));
        } else {
            showDebugMessage(QString("Could not find Windows app icon at: %1").arg(icoPath));
            // The icon should still be loaded from the embedded resource
        }
        
        // If no icon was loaded, Windows will use the one from the RC file
#elif defined(Q_OS_MAC)
        // macOS-specific icon - try multiple locations and formats
        QStringList macIconPaths = {
            QDir(QCoreApplication::applicationDirPath()).filePath("icons/appicons/app_icon_mac.icns"),
            QDir(QCoreApplication::applicationDirPath()).filePath("icons/appicons/notepadx_mac@2x.png"),
            QDir(QCoreApplication::applicationDirPath()).filePath("icons/appicons/notepadx_mac.png"),
            QDir(QDir::currentPath()).filePath("icons/appicons/app_icon_mac.icns"),
            QDir(QDir::currentPath()).filePath("icons/appicons/notepadx_mac@2x.png")
        };
        
        bool iconLoaded = false;
        for (const QString &path : macIconPaths) {
            if (QFile::exists(path)) {
                appIcon = QIcon(path);
                showDebugMessage(QString("Loaded macOS app icon from: %1").arg(path));
                iconLoaded = true;
                break;
            }
        }
        
        if (!iconLoaded) {
            showDebugMessage("Could not find macOS app icon in any of the expected locations");
        }
#else
        // Linux/other platforms icon - try multiple locations and formats
        QStringList linuxIconPaths = {
            QDir(QCoreApplication::applicationDirPath()).filePath("icons/appicons/app_icon_linux.png"),
            QDir(QCoreApplication::applicationDirPath()).filePath("icons/appicons/notepadx_linux.png"),
            QDir(QDir::currentPath()).filePath("icons/appicons/app_icon_linux.png"),
            QDir(QDir::currentPath()).filePath("icons/appicons/notepadx_linux.png"),
            // Standard Linux icon locations
            QDir("/usr/share/icons/hicolor/256x256/apps").filePath("notepadx.png"),
            QDir("/usr/share/pixmaps").filePath("notepadx.png")
        };
        
        bool iconLoaded = false;
        for (const QString &path : linuxIconPaths) {
            if (QFile::exists(path)) {
                appIcon = QIcon(path);
                showDebugMessage(QString("Loaded Linux app icon from: %1").arg(path));
                iconLoaded = true;
                break;
            }
        }
        
        if (!iconLoaded) {
            showDebugMessage("Could not find Linux app icon in any of the expected locations");
            
            // On Linux, try the system theme as fallback
            if (QIcon::hasThemeIcon("accessories-text-editor")) {
                appIcon = QIcon::fromTheme("accessories-text-editor");
                showDebugMessage("Using system theme icon for text editor");
                iconLoaded = true;
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
        
        showDebugMessage("Creating main window");
        
        try {
            MainWindow window;
            showDebugMessage("Main window created");
            
            window.show();
            showDebugMessage("Window shown");
            
            return app.exec();
        }
        catch (const std::exception& e) {
            showDebugMessage(QString("Exception in main window: %1").arg(e.what()));
            QMessageBox::critical(nullptr, "Error", QString("Critical error: %1").arg(e.what()));
            return 1;
        }
        catch (...) {
            showDebugMessage("Unknown exception in main window");
            QMessageBox::critical(nullptr, "Error", "Unknown critical error occurred");
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        QFile file("crash_log.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream stream(&file);
            stream << "FATAL EXCEPTION: " << e.what() << "\n";
            file.close();
        }
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown exception" << std::endl;
        QFile file("crash_log.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream stream(&file);
            stream << "FATAL UNKNOWN EXCEPTION\n";
            file.close();
        }
        return 1;
    }
}
