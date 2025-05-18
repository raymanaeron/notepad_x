#include "editormanager.h"
#include "mainwindow.h"
#include "editorwidget.h"
#include "codeeditor.h"
#include "fileoperations.h"
#include "highlighting/highlighterfactory.h"
#include <QTabWidget>
#include <QSettings>
#include <QStatusBar>
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QPalette>
#include <QPixmap>
#include <QPainter>
#include <QFileInfo>
#include <QRegularExpression>
#include <QMenuBar>
#include <QLabel> // Add QLabel header to fix incomplete type errors

EditorManager::EditorManager(MainWindow *parent)
    : QObject(parent), m_mainWindow(parent), m_currentZoomLevel(0)
{
    // Initialize with safe defaults
    m_isDarkThemeActive = false;
    m_isWordWrapEnabled = false;
    m_languageMenu = nullptr;
    m_languageActionGroup = nullptr;
    m_themeActionGroup = nullptr;
    m_wordWrapAction = nullptr;
    
    // Get status bar labels directly from MainWindow using the getter methods
    m_lineColumnLabel = parent->getLineColumnLabel();
    m_modifiedLabel = parent->getModifiedLabel();
    m_filenameLabel = parent->getFilenameLabel();
    m_zoomLabel = parent->getZoomLabel();
    
    // Initialize member variables with values from MainWindow
    m_tabWidget = parent->findChild<QTabWidget*>();
    if (!m_tabWidget) {
        qWarning("EditorManager: Failed to find tab widget!");
        return;
    }
    
    // Load settings early as they're needed for initialization
    QSettings settings("NotepadX", "Editor");
    m_isDarkThemeActive = settings.value("darkTheme", false).toBool();
    m_isWordWrapEnabled = settings.value("wordWrap", false).toBool();
    
    // Make sure we have a menu bar
    QMenuBar* menuBar = parent->menuBar();
    if (!menuBar) {
        qWarning("EditorManager: MenuBar is null!");
        return; // Skip the rest of initialization
    }
    
    // Find language menu from the menu bar actions
    for (QAction* menuAction : menuBar->actions()) {
        if (menuAction->text() == "&Language" && menuAction->menu()) {
            m_languageMenu = menuAction->menu();
            break;
        }
    }
    
    // Find word wrap action - more direct approach
    for (QAction* action : parent->findChildren<QAction*>()) {
        if (action->text() == "&Word Wrap" && action->isCheckable()) {
            m_wordWrapAction = action;
            break;
        }
    }
    
    // Synchronize the word wrap checkbox with the loaded setting
    if (m_wordWrapAction) {
        m_wordWrapAction->setChecked(m_isWordWrapEnabled);
    }
    
    // Apply word wrap setting to any existing editors
    QTextOption::WrapMode mode = m_isWordWrapEnabled ? 
        QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::NoWrap;
    
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget *editor = qobject_cast<EditorWidget *>(m_tabWidget->widget(i));
        if (editor) {
            editor->setWordWrapMode(mode);
        }
    }
}

EditorManager::~EditorManager()
{
}

void EditorManager::applyLightTheme()
{
    qApp->setStyle("Fusion");
    m_mainWindow->setStyleSheet("");

    QPalette lightPalette;
    lightPalette.setColor(QPalette::Window, QColor(240, 240, 240));
    lightPalette.setColor(QPalette::WindowText, QColor(0, 0, 0));
    lightPalette.setColor(QPalette::Base, QColor(255, 255, 255));
    lightPalette.setColor(QPalette::AlternateBase, QColor(245, 245, 245));
    lightPalette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));
    lightPalette.setColor(QPalette::ToolTipText, QColor(0, 0, 0));
    lightPalette.setColor(QPalette::Text, QColor(0, 0, 0));
    lightPalette.setColor(QPalette::Button, QColor(240, 240, 240));
    lightPalette.setColor(QPalette::ButtonText, QColor(0, 0, 0));
    lightPalette.setColor(QPalette::BrightText, QColor(0, 0, 0));
    lightPalette.setColor(QPalette::Link, QColor(0, 122, 204));
    lightPalette.setColor(QPalette::Highlight, QColor(185, 215, 255));
    lightPalette.setColor(QPalette::HighlightedText, QColor(0, 0, 0));

    qApp->setPalette(lightPalette);

    m_isDarkThemeActive = false;

    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget *editor = qobject_cast<EditorWidget *>(m_tabWidget->widget(i));
        if (editor) {
            editor->setLightTheme();
        }
    }
    
    // Set toolbar icons to appropriate color for light theme
    QList<QAction *> actions = m_mainWindow->findChildren<QAction *>();
    for (QAction *action : actions) {
        if (action->icon().isNull())
            continue;

        QIcon originalIcon = action->icon();
        QIcon::Mode mode = QIcon::Normal;

        QList<QSize> sizes = originalIcon.availableSizes(mode);
        if (sizes.isEmpty())
            sizes.append(QSize(24, 24));

        QIcon newIcon;
        for (const QSize &size : sizes) {
            QPixmap pixmap = originalIcon.pixmap(size);

            QPainter painter(&pixmap);
            painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            painter.fillRect(pixmap.rect(), QColor(50, 50, 50));  // Dark color for light theme
            painter.end();

            newIcon.addPixmap(pixmap, mode);
        }

        action->setIcon(newIcon);
    }

    QSettings settings("NotepadX", "Editor");
    settings.setValue("darkTheme", m_isDarkThemeActive);
}

void EditorManager::applyDarkTheme()
{
    qApp->setStyle("Fusion");

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(51, 51, 51));
    darkPalette.setColor(QPalette::WindowText, QColor(220, 220, 220));
    darkPalette.setColor(QPalette::Base, QColor(30, 30, 30));
    darkPalette.setColor(QPalette::AlternateBase, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(51, 51, 51));
    darkPalette.setColor(QPalette::ToolTipText, QColor(220, 220, 220));
    darkPalette.setColor(QPalette::Text, QColor(220, 220, 220));
    darkPalette.setColor(QPalette::Button, QColor(51, 51, 51));
    darkPalette.setColor(QPalette::ButtonText, QColor(220, 220, 220));
    darkPalette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    darkPalette.setColor(QPalette::Link, QColor(0, 122, 204));
    darkPalette.setColor(QPalette::Highlight, QColor(0, 122, 204));
    darkPalette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));

    qApp->setPalette(darkPalette);
    
    // Set toolbar icons to bright white for better visibility in dark theme
    QList<QAction *> actions = m_mainWindow->findChildren<QAction *>();
    for (QAction *action : actions) {
        if (action->icon().isNull())
            continue;

        QIcon originalIcon = action->icon();
        QIcon::Mode mode = QIcon::Normal;

        QList<QSize> sizes = originalIcon.availableSizes(mode);
        if (sizes.isEmpty())
            sizes.append(QSize(24, 24));

        QIcon newIcon;
        for (const QSize &size : sizes) {
            QPixmap pixmap = originalIcon.pixmap(size);

            QPainter painter(&pixmap);
            painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            painter.fillRect(pixmap.rect(), QColor(255, 255, 255));  // Pure white for dark theme
            painter.end();

            newIcon.addPixmap(pixmap, mode);
        }

        action->setIcon(newIcon);
    }

    // Add styling specifically for tab alignment in dark mode
    QString styleSheet = QString(
        "QMenuBar { background-color: rgb(73, 73, 73); color: rgb(220, 220, 220); } "
        "QMenuBar::item:selected { background-color: rgb(60, 60, 60); } "
        "QMenu { background-color: rgb(48, 48, 48); color: rgb(220, 220, 220); border: 1px solid rgb(64, 64, 64); } "
        "QMenu::item:selected { background-color: rgb(89, 97, 207); } "
        "QToolBar { background-color: rgb(65,65,65); border: none; } "
        "QToolBar::separator { background-color: rgb(80, 80, 80); width: 1px; margin: 4px 4px; } "
        "QStatusBar { background-color: rgb(51, 51, 51); color: rgb(220, 220, 220); }"
        "QTabBar::tab { background-color: rgb(51, 51, 51); color: rgb(220, 220, 220); border: 1px solid rgb(64, 64, 64); padding: 5px; }"
        "QTabBar::tab:selected { background-color: rgb(30, 30, 30); }"
    );

    m_mainWindow->setStyleSheet(styleSheet);

    m_isDarkThemeActive = true;

    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget *editor = qobject_cast<EditorWidget *>(m_tabWidget->widget(i));
        if (editor) {
            editor->setDarkTheme();
        }
    }

    QSettings settings("NotepadX", "Editor");
    settings.setValue("darkTheme", m_isDarkThemeActive);
}

void EditorManager::toggleWordWrap()
{
    m_isWordWrapEnabled = !m_isWordWrapEnabled;
    
    // Update menu item state
    if (m_wordWrapAction) {
        m_wordWrapAction->setChecked(m_isWordWrapEnabled);
    }
    
    // Apply word wrap setting to all open editors
    QTextOption::WrapMode mode = m_isWordWrapEnabled ? 
        QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::NoWrap;
    
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget *editor = qobject_cast<EditorWidget *>(m_tabWidget->widget(i));
        if (editor) {
            editor->setWordWrapMode(mode);
        }
    }
      // Save the setting
    QSettings settings("NotepadX", "Editor");
    settings.setValue("wordWrap", m_isWordWrapEnabled);
    
    // Notify FileOperations about the change so it's consistent for new tabs
    auto fileOps = m_mainWindow->findChild<FileOperations*>();
    if (fileOps) {
        fileOps->setWordWrapEnabled(m_isWordWrapEnabled);
    }
}

void EditorManager::updateLanguageMenu()
{
    EditorWidget *editor = currentEditor();
    if (!editor) {
        return; // No editor open
    }
    
    if (!m_languageActionGroup) {
        // If the language action group isn't set yet, try to find it in the parent window
        QList<QActionGroup*> actionGroups = m_mainWindow->findChildren<QActionGroup*>();
        for (QActionGroup* group : actionGroups) {
            // Check if this group has actions that match language names
            QList<QAction*> actions = group->actions();
            if (!actions.isEmpty() && actions.contains(group->checkedAction()) && 
                (actions.first()->text() == "None" || actions.first()->text() == "Plain Text")) {
                m_languageActionGroup = group;
                break;
            }
        }
    }
    
    if (editor && m_languageActionGroup) {
        QString currentLang = editor->currentLanguage();

        for (QAction *action : m_languageActionGroup->actions()) {
            if (action->text() == currentLang) {
                action->setChecked(true);
                break;
            }
        }
    }
}

void EditorManager::languageSelected(QAction *action)
{
    EditorWidget *editor = currentEditor();
    if (editor) {
        editor->setLanguage(action->text());
    }
}

void EditorManager::zoomIn()
{
    EditorWidget *editor = currentEditor();
    if (editor) {
        editor->zoomIn();
        updateStatusBar();
    }
}

void EditorManager::zoomOut()
{
    EditorWidget *editor = currentEditor();
    if (editor) {
        editor->zoomOut();
        updateStatusBar();
    }
}

void EditorManager::resetZoom()
{
    EditorWidget *editor = currentEditor();
    if (editor) {
        editor->resetZoom();
        updateStatusBar();
    }
}

EditorWidget *EditorManager::currentEditor()
{
    return qobject_cast<EditorWidget *>(m_tabWidget->currentWidget());
}

void EditorManager::connectEditorSignals()
{
    EditorWidget *editor = currentEditor();
    if (!editor)
        return;

    CodeEditor *codeEditor = editor->editor();
    if (codeEditor) {
        disconnect(codeEditor, &CodeEditor::cursorPositionChanged, nullptr, nullptr);
        connect(codeEditor, &CodeEditor::cursorPositionChanged, this, &EditorManager::updateCursorPosition);
        
        disconnect(codeEditor, &CodeEditor::zoomLevelChanged, nullptr, nullptr);
        connect(codeEditor, &CodeEditor::zoomLevelChanged, this, [this](int) {
            updateStatusBar();
        });
    }

    disconnect(editor, &EditorWidget::zoomLevelChanged, nullptr, nullptr);
    connect(editor, &EditorWidget::zoomLevelChanged, this, [this](int) {
        updateStatusBar();
    });
}

void EditorManager::updateTabText(int index)
{
    if (index < 0)
        return;

    EditorWidget *editor = qobject_cast<EditorWidget *>(m_tabWidget->widget(index));
    if (!editor)
        return;

    if (index == m_tabWidget->currentIndex()) {
        QString title = editor->currentFile();
        int untitledCount = 0;
        
        // Find the untitled count from the tab name if this is an untitled document
        if (title.isEmpty()) {
            QString currentTabText = m_tabWidget->tabText(index);
            QRegularExpression regex("Untitled (\\d+)");
            QRegularExpressionMatch match = regex.match(currentTabText);
            if (match.hasMatch()) {
                untitledCount = match.captured(1).toInt();
            }
            title = QString("Untitled %1").arg(untitledCount);
        }
        else {
            title = QFileInfo(title).fileName();
        }

        if (editor->isModified())
            title += " *";

        title += " - NotepadX";
        m_mainWindow->setWindowTitle(title);
    }
}

void EditorManager::documentModified(bool modified)
{
    EditorWidget *editor = qobject_cast<EditorWidget *>(sender());
    if (!editor)
        return;

    int index = m_tabWidget->indexOf(editor);
    if (index < 0)
        return;

    QString tabText = m_tabWidget->tabText(index);
    if (modified && !tabText.endsWith(" *")) {
        m_tabWidget->setTabText(index, tabText + " *");
    } else if (!modified && tabText.endsWith(" *")) {
        m_tabWidget->setTabText(index, tabText.left(tabText.length() - 2));
    }

    if (index == m_tabWidget->currentIndex()) {
        updateTabText(index);
        updateStatusBar();
    }
}

void EditorManager::updateStatusBar()
{
    EditorWidget *editor = currentEditor();
    if (!editor) {
        m_lineColumnLabel->setText("Line: 1, Column: 1");
        m_zoomLabel->setText("Zoom: 100%");
        m_modifiedLabel->setText("");
        m_filenameLabel->setText("");
        return;
    }

    if (editor->isModified()) {
        m_modifiedLabel->setText("[Modified]");
    } else {
        m_modifiedLabel->setText("");
    }

    QString filename = editor->currentFile();
    if (filename.isEmpty()) {
        QString tabText = m_tabWidget->tabText(m_tabWidget->currentIndex());
        m_filenameLabel->setText(tabText);
    } else {
        m_filenameLabel->setText(filename);
    }

    int zoomLevel = editor->getCurrentZoomLevel();
    int zoomPercent = 100 + (zoomLevel * 10);
    m_zoomLabel->setText(QString("Zoom: %1%").arg(zoomPercent));

    CodeEditor *codeEditor = editor->editor();
    if (codeEditor) {
        QTextCursor cursor = codeEditor->textCursor();
        int line = cursor.blockNumber() + 1;
        int column = cursor.columnNumber() + 1;
        m_lineColumnLabel->setText(QString("Line: %1, Column: %2").arg(line).arg(column));
    }
}

void EditorManager::updateCursorPosition()
{
    EditorWidget *editor = currentEditor();
    if (!editor)
        return;

    CodeEditor *codeEditor = editor->editor();
    if (codeEditor) {
        QTextCursor cursor = codeEditor->textCursor();
        int line = cursor.blockNumber() + 1;
        int column = cursor.columnNumber() + 1;
        m_lineColumnLabel->setText(QString("Line: %1, Column: %2").arg(line).arg(column));
    }
}

void EditorManager::updateWordWrapState()
{
    // Read the current word wrap setting
    QSettings settings("NotepadX", "Editor");
    m_isWordWrapEnabled = settings.value("wordWrap", false).toBool();
    
    // Update the action state
    if (m_wordWrapAction) {
        m_wordWrapAction->setChecked(m_isWordWrapEnabled);
    }
    
    // Apply word wrap setting to all open editors
    QTextOption::WrapMode mode = m_isWordWrapEnabled ? 
        QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::NoWrap;
    
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget *editor = qobject_cast<EditorWidget *>(m_tabWidget->widget(i));
        if (editor) {
            editor->setWordWrapMode(mode);
        }
    }
    
    // Also notify FileOperations about the current setting
    auto fileOps = m_mainWindow->findChild<FileOperations*>();
    if (fileOps) {
        fileOps->setWordWrapEnabled(m_isWordWrapEnabled);
    }
}