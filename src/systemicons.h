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
        // macOS-specific icons
        if (name == "file")
            return QApplication::style()->standardIcon(QStyle::SP_FileIcon);
        else if (name == "open")
            return QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton);
        else if (name == "save")
            return QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton);
        else if (name == "cut")
            return QIcon::fromTheme("edit-cut", QApplication::style()->standardIcon(QStyle::SP_FileIcon));
        else if (name == "copy")
            return QIcon::fromTheme("edit-copy", QApplication::style()->standardIcon(QStyle::SP_FileIcon));
        else if (name == "paste")
            return QIcon::fromTheme("edit-paste", QApplication::style()->standardIcon(QStyle::SP_FileIcon));
        else if (name == "undo")
            return QIcon::fromTheme("edit-undo", QApplication::style()->standardIcon(QStyle::SP_ArrowBack));
        else if (name == "redo")
            return QIcon::fromTheme("edit-redo", QApplication::style()->standardIcon(QStyle::SP_ArrowForward));
#else
        // Linux/Unix use theme icons
        QString iconName;
        if (name == "file")
            iconName = "document-new";
        else if (name == "open")
            iconName = "document-open";
        else if (name == "save")
            iconName = "document-save";
        else if (name == "cut")
            iconName = "edit-cut";
        else if (name == "copy")
            iconName = "edit-copy";
        else if (name == "paste")
            iconName = "edit-paste";
        else if (name == "undo")
            iconName = "edit-undo";
        else if (name == "redo")
            iconName = "edit-redo";
        else
            iconName = name;
            
        return QIcon::fromTheme(iconName, QApplication::style()->standardIcon(QStyle::SP_FileIcon));
#endif
        // Default fallback
        return QIcon();
    }
};

#endif // SYSTEMICONS_H
