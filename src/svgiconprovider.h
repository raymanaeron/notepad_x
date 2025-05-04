#ifndef SVGICONPROVIDER_H
#define SVGICONPROVIDER_H

#include <QIcon>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QApplication>
#include <QPalette>
#include <QSvgRenderer>
#include <QPainter>

class SvgIconProvider
{
public:
    // Get SVG icon by name
    static QIcon getIcon(const QString &iconName)
    {
        // Try to load SVG from resources or local path
        QString svgContent;
        QString iconPath = QString(":/icons/%1.svg").arg(iconName);
        
        if (!loadSvgContent(iconPath, svgContent)) {
            // Try local path
            QString localPath = QDir(QCoreApplication::applicationDirPath()).filePath(
                QString("icons/%1.svg").arg(iconName));
            
            if (!loadSvgContent(localPath, svgContent)) {
                qDebug() << "SVG icon not found:" << iconName;
                return QIcon();
            }
        }
        
        // Create pixmaps for different theme states
        QIcon icon;
        
        // For light theme - use dark color
        QPixmap lightThemePixmap = createColoredPixmap(svgContent, QColor(40, 40, 40));
        icon.addPixmap(lightThemePixmap, QIcon::Normal, QIcon::Off);
        
        // For dark theme - use light color
        QPixmap darkThemePixmap = createColoredPixmap(svgContent, QColor(220, 220, 220));
        icon.addPixmap(darkThemePixmap, QIcon::Normal, QIcon::On);
        
        // Create disabled state pixmaps
        QPixmap lightDisabledPixmap = createColoredPixmap(svgContent, QColor(150, 150, 150));
        icon.addPixmap(lightDisabledPixmap, QIcon::Disabled, QIcon::Off);
        
        QPixmap darkDisabledPixmap = createColoredPixmap(svgContent, QColor(120, 120, 120));
        icon.addPixmap(darkDisabledPixmap, QIcon::Disabled, QIcon::On);
        
        return icon;
    }

private:
    // Load SVG content from file
    static bool loadSvgContent(const QString &path, QString &content)
    {
        QFile file(path);
        if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
            return false;
        }
        
        content = QString::fromUtf8(file.readAll());
        file.close();
        return true;
    }
    
    // Create colored pixmap from SVG content
    static QPixmap createColoredPixmap(const QString &svgContent, const QColor &color)
    {
        // Prepare SVG content with color
        QString coloredSvg = svgContent;
        coloredSvg.replace("currentColor", color.name());
        
        // Render SVG to pixmap
        QSvgRenderer renderer(coloredSvg.toUtf8());
        QPixmap pixmap(24, 24);
        pixmap.fill(Qt::transparent);
        
        QPainter painter(&pixmap);
        renderer.render(&painter);
        
        return pixmap;
    }
};

#endif // SVGICONPROVIDER_H
