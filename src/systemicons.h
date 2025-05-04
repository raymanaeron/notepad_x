#ifndef SYSTEMICONS_H
#define SYSTEMICONS_H

#include <QIcon>
#include <QStyle>
#include <QApplication>

class SystemIcons
{
public:
    static QIcon getIcon(const QString &name, QColor color = QColor())
    {
#ifdef Q_OS_WIN
        // Windows-specific icons
        if (name == "file")
            return QApplication::style()->standardIcon(QStyle::SP_FileIcon);
        else if (name == "open")
            return QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton);
        else if (name == "save")
            return QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton);
        else if (name == "cut")
            return QApplication::style()->standardIcon(QStyle::SP_DialogResetButton); // Placeholder
        else if (name == "copy")
            return QApplication::style()->standardIcon(QStyle::SP_DialogHelpButton); // Placeholder
        else if (name == "paste")
            return QApplication::style()->standardIcon(QStyle::SP_DialogApplyButton); // Placeholder
        else if (name == "undo")
            return QApplication::style()->standardIcon(QStyle::SP_ArrowBack);
        else if (name == "redo")
            return QApplication::style()->standardIcon(QStyle::SP_ArrowForward);
#elif defined(Q_OS_MAC)
        // macOS-specific icons would go here
#else
        // Linux/Unix would use theme icons
        return QIcon::fromTheme(name);
#endif
        // Default fallback
        return QIcon();
    }
};

#endif // SYSTEMICONS_H
