#ifndef SVGICONPROVIDER_H
#define SVGICONPROVIDER_H

#include <QIcon>
#include <QString>

class SvgIconProvider
{
public:
    // Get SVG icon by name
    static QIcon getIcon(const QString &iconName)
    {
        return QIcon(QString(":/icons/%1.svg").arg(iconName));
    }
};

#endif // SVGICONPROVIDER_H
