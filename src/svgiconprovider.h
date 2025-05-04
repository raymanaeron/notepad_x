#ifndef SVGICONPROVIDER_H
#define SVGICONPROVIDER_H

#include <QIcon>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QCoreApplication>

class SvgIconProvider
{
public:
    // Get SVG icon by name
    static QIcon getIcon(const QString &iconName)
    {
        // First try from resources
        QString iconPath = QString(":/icons/%1.svg").arg(iconName);
        qDebug() << "Trying to load SVG from resources:" << iconPath;
        
        if (QFile::exists(iconPath)) {
            qDebug() << "Found SVG in resources";
            return QIcon(iconPath);
        }
        
        // If not found in resources, try from local directory
        QString localPath = QDir(QCoreApplication::applicationDirPath()).filePath(
            QString("icons/%1.svg").arg(iconName));
        qDebug() << "Trying to load SVG from local path:" << localPath;
        
        if (QFile::exists(localPath)) {
            qDebug() << "Found SVG in local path";
            return QIcon(localPath);
        }
        
        qDebug() << "SVG icon not found in resources or local path";
        return QIcon(); // Return empty icon
    }
};

#endif // SVGICONPROVIDER_H
