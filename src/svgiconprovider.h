#ifndef SVGICONPROVIDER_H
#define SVGICONPROVIDER_H

#include <QIcon>
#include <QString>
#include <QDebug>
#include <QFile>

class SvgIconProvider
{
public:
    // Get SVG icon by name
    static QIcon getIcon(const QString &iconName)
    {
        QString iconPath = QString(":/icons/%1.svg").arg(iconName);
        
        try {
            // Verify file exists
            QFile file(iconPath);
            if (!file.exists()) {
                qDebug() << "SVG file not found:" << iconPath;
                return QIcon();
            }
            
            // Return a simple QIcon without SVG validation
            // This avoids crashes if QtSvg module has issues
            return QIcon(iconPath);
        }
        catch (const std::exception& e) {
            qDebug() << "Exception while loading SVG:" << e.what();
            return QIcon();
        }
        catch (...) {
            qDebug() << "Unknown exception while loading SVG";
            return QIcon();
        }
    }
};

#endif // SVGICONPROVIDER_H
