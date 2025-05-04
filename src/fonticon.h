#ifndef FONTICON_H
#define FONTICON_H

#include <QString>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QFontDatabase>
#include <QApplication>

class FontIcon
{
public:
    // Font Awesome 4 icon codes
    enum Icon {
        FA_FILE = 0xf15b,
        FA_FOLDER_OPEN = 0xf07c,
        FA_SAVE = 0xf0c7,
        FA_CUT = 0xf0c4,
        FA_COPY = 0xf0c5,
        FA_PASTE = 0xf0ea,
        FA_UNDO = 0xf0e2,
        FA_REPEAT = 0xf01e,  // For redo
        FA_SEARCH = 0xf002,
        FA_PLUS = 0xf067,
        FA_MINUS = 0xf068,
        FA_REFRESH = 0xf021  // For reset
    };
    
    static QIcon icon(Icon iconCode, const QColor &color = QColor(50, 50, 50), int size = 16)
    {
        // Try to get FontAwesome font
        QFont font("FontAwesome");
        if (!font.exactMatch()) {
            // Try to find it in the database
            QStringList fontFamilies = QFontDatabase::families();
            for (const QString &family : fontFamilies) {
                if (family.contains("awesome", Qt::CaseInsensitive)) {
                    font.setFamily(family);
                    break;
                }
            }
        }
        
        font.setPixelSize(size);
        
        // Create a pixmap to render the icon
        QPixmap pixmap(size, size);
        pixmap.fill(Qt::transparent);
        
        QPainter painter(&pixmap);
        painter.setFont(font);
        painter.setPen(color);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);
        
        // Convert the enum to uint16_t/char16_t explicitly before creating QChar
        QString text = QString(QChar(static_cast<uint16_t>(iconCode)));
        painter.drawText(pixmap.rect(), Qt::AlignCenter | Qt::TextDontClip, text);
        
        return QIcon(pixmap);
    }
    
    // Helper function for system icons (platform specific)
    static QIcon systemIcon(const QString &iconName)
    {
        // This is a placeholder for a more sophisticated approach
        // that would use system-specific icon themes
        return QIcon::fromTheme(iconName);
    }
};

#endif // FONTICON_H
