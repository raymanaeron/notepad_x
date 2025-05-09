#include "fileoperations.h"
#include "mainwindow.h"
#include "editorwidget.h"
#include "codeeditor.h" 
#include <QTabWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QApplication>
#include <QMenuBar>
#include <QDebug>

FileOperations::FileOperations(MainWindow *parent)
    : QObject(parent), m_mainWindow(parent), m_untitledCount(0), 
      m_recentFilesMenu(nullptr), m_isDarkThemeActive(false), m_isWordWrapEnabled(false)
{
    // Initialize member variables with values from MainWindow
    m_tabWidget = parent->findChild<QTabWidget*>();
    if (!m_tabWidget) {
        qWarning("FileOperations: Could not find tab widget!");
        return;
    }
    
    // Safer way to get menus - only do this if menus exist
    QMenuBar* menuBar = parent->menuBar();
    if (!menuBar) {
        qWarning("FileOperations: Menu bar not found");
        return;
    }
    
    // First try to find the File menu from the menu bar actions
    QMenu* fileMenu = nullptr;
    for (QAction* action : menuBar->actions()) {
        if (action->text() == "&File" && action->menu()) {
            fileMenu = action->menu();
            break;
        }
    }
    
    // Then find the Recent Files submenu
    if (fileMenu) {
        for (QAction* action : fileMenu->actions()) {
            if (action->menu() && action->text() == "Recent &Files") {
                m_recentFilesMenu = action->menu();
                break;
            }
        }
    }
    
    // Read settings for dark theme and word wrap
    QSettings settings("NotepadX", "Editor");
    m_isDarkThemeActive = settings.value("darkTheme", false).toBool();
    m_isWordWrapEnabled = settings.value("wordWrap", false).toBool();
    
    // Load recent files from settings
    QStringList validRecentFiles;
    recentFiles = settings.value("recentFiles").toStringList();
    for (const QString &filePath : recentFiles) {
        if (QFile::exists(filePath)) {
            validRecentFiles.append(filePath);
        }
    }
    recentFiles = validRecentFiles;
    
    // Only update menu if we found it
    if (m_recentFilesMenu) {
        updateRecentFilesMenu();
    }
}

FileOperations::~FileOperations()
{
}

void FileOperations::createNewTab()
{
    EditorWidget *editor = createEditor();

    QString tabName = QString("Untitled %1").arg(++m_untitledCount);
    int index = m_tabWidget->addTab(editor, tabName);

    connect(editor, &EditorWidget::fileNameChanged, m_mainWindow, [=](const QString &fileName) {
        QString tabText = fileName.isEmpty() ? 
                        QString("Untitled %1").arg(m_untitledCount) : 
                        QFileInfo(fileName).fileName();
        m_tabWidget->setTabText(m_tabWidget->indexOf(editor), tabText);
        
        // Using QMetaObject::invokeMethod to call MainWindow's slots
        QMetaObject::invokeMethod(m_mainWindow, "updateStatusBar");
    });

    // Fixed connection syntax using pointer to member function
    connect(editor, &EditorWidget::modificationChanged, m_mainWindow, &MainWindow::documentModified);

    connect(editor, &EditorWidget::languageChanged, m_mainWindow, [=](const QString &) {
        QMetaObject::invokeMethod(m_mainWindow, "updateLanguageMenu");
    });

    connect(editor, &EditorWidget::zoomLevelChanged, m_mainWindow, [=](int) {
        QMetaObject::invokeMethod(m_mainWindow, "updateStatusBar");
    });

    m_tabWidget->setCurrentIndex(index);
    editor->setFocus();

    if (m_isDarkThemeActive) {
        editor->setDarkTheme();
    } else {
        editor->setLightTheme();
    }

    QMetaObject::invokeMethod(m_mainWindow, "updateLanguageMenu");
    QMetaObject::invokeMethod(m_mainWindow, "connectEditorSignals");
}

EditorWidget* FileOperations::createEditor()
{
    EditorWidget *editor = new EditorWidget(m_mainWindow);

    // Apply current word wrap setting
    editor->setWordWrapMode(m_isWordWrapEnabled ? QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::NoWrap);

    return editor;
}

void FileOperations::closeCurrentTab()
{
    int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex != -1) {
        onTabCloseRequested(currentIndex);
    }
}

void FileOperations::onTabCloseRequested(int index)
{
    EditorWidget *editor = qobject_cast<EditorWidget *>(m_tabWidget->widget(index));
    if (!editor)
        return;

    if (editor->maybeSave()) {
        m_tabWidget->removeTab(index);
        delete editor;

        if (m_tabWidget->count() == 0) {
            createNewTab();
        }
    }
}

void FileOperations::openFile()
{
    if (!ensureHasOpenTab())
        return;

    QString fileName = QFileDialog::getOpenFileName(m_mainWindow);
    if (fileName.isEmpty())
        return;

    if (openFileHelper(fileName)) {
        addToRecentFiles(fileName);
    }
}

bool FileOperations::openFileHelper(const QString &fileName)
{
    if (fileName.isEmpty())
        return false;

    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget *editor = qobject_cast<EditorWidget *>(m_tabWidget->widget(i));
        if (editor && QFileInfo(fileName) == QFileInfo(editor->currentFile())) {
            m_tabWidget->setCurrentIndex(i);
            return true;
        }
    }

    // Get the current editor
    int currentIndex = m_tabWidget->currentIndex();
    EditorWidget *currentEditorWidget = nullptr;
    if (currentIndex >= 0) {
        currentEditorWidget = qobject_cast<EditorWidget *>(m_tabWidget->widget(currentIndex));
    }

    if (currentEditorWidget && currentEditorWidget->isUntitled() && !currentEditorWidget->isModified()) {
        if (currentEditorWidget->loadFile(fileName)) {
            m_mainWindow->statusBar()->showMessage(QObject::tr("File loaded"), 2000);
            return true;
        }
        return false;
    }

    EditorWidget *editor = createEditor();
    if (editor->loadFile(fileName)) {
        int index = m_tabWidget->addTab(editor, QFileInfo(fileName).fileName());
        m_tabWidget->setCurrentIndex(index);

        connect(editor, &EditorWidget::fileNameChanged, m_mainWindow, [=](const QString &fileName) {
            QString tabText = fileName.isEmpty() ? 
                            QString("Untitled %1").arg(m_untitledCount) : 
                            QFileInfo(fileName).fileName();
            m_tabWidget->setTabText(m_tabWidget->indexOf(editor), tabText);
            QMetaObject::invokeMethod(m_mainWindow, "updateStatusBar");
        });

        // Fixed connection syntax
        connect(editor, &EditorWidget::modificationChanged, m_mainWindow, &MainWindow::documentModified);

        connect(editor, &EditorWidget::zoomLevelChanged, m_mainWindow, [=](int) {
            QMetaObject::invokeMethod(m_mainWindow, "updateStatusBar");
        });

        m_mainWindow->statusBar()->showMessage(QObject::tr("File loaded"), 2000);

        if (m_isDarkThemeActive) {
            editor->setDarkTheme();
        } else {
            editor->setLightTheme();
        }

        QMetaObject::invokeMethod(m_mainWindow, "connectEditorSignals");

        return true;
    } else {
        delete editor;
        return false;
    }
}

bool FileOperations::saveFile()
{
    if (!ensureHasOpenTab())
        return false;

    int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex < 0)
        return false;

    EditorWidget *editor = qobject_cast<EditorWidget *>(m_tabWidget->widget(currentIndex));
    if (!editor)
        return false;

    if (editor->save()) {
        m_mainWindow->statusBar()->showMessage(QObject::tr("File saved"), 2000);
        if (!editor->currentFile().isEmpty()) {
            addToRecentFiles(editor->currentFile());
        }
        return true;
    }

    return false;
}

bool FileOperations::saveFileAs()
{
    if (!ensureHasOpenTab())
        return false;

    int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex < 0)
        return false;

    EditorWidget *editor = qobject_cast<EditorWidget *>(m_tabWidget->widget(currentIndex));
    if (!editor)
        return false;

    if (editor->saveAs()) {
        m_mainWindow->statusBar()->showMessage(QObject::tr("File saved"), 2000);
        if (!editor->currentFile().isEmpty()) {
            addToRecentFiles(editor->currentFile());
        }
        return true;
    }

    return false;
}

bool FileOperations::ensureHasOpenTab()
{
    if (m_tabWidget->count() == 0) {
        createNewTab();
    }
    return m_tabWidget->count() > 0;
}

bool FileOperations::maybeSaveAll()
{
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget *editor = qobject_cast<EditorWidget *>(m_tabWidget->widget(i));
        if (editor && editor->isModified()) {
            m_tabWidget->setCurrentIndex(i);
            if (!editor->maybeSave()) {
                return false;
            }
        }
    }
    return true;
}

void FileOperations::addToRecentFiles(const QString &filePath)
{
    recentFiles.removeAll(filePath);
    recentFiles.prepend(filePath);
    
    // Limit to MAX_RECENT_FILES
    while (recentFiles.size() > MAX_RECENT_FILES) {
        recentFiles.removeLast();
    }
    
    updateRecentFilesMenu();
}

void FileOperations::openRecentFile(const QString &filePath)
{
    if (QFile::exists(filePath)) {
        openFileHelper(filePath);
    }
}

void FileOperations::clearRecentFiles()
{
    recentFiles.clear();
    updateRecentFilesMenu();
}

void FileOperations::updateRecentFilesMenu()
{
    if (!m_recentFilesMenu)
        return;

    m_recentFilesMenu->clear();
    
    // First add the "Clear Recent Files" action
    QAction *clearRecentAction = new QAction("&Clear Recent Files", m_mainWindow);
    connect(clearRecentAction, &QAction::triggered, this, &FileOperations::clearRecentFiles);
    m_recentFilesMenu->addAction(clearRecentAction);
    
    if (!recentFiles.isEmpty()) {
        m_recentFilesMenu->addSeparator();
        
        for (const QString &file : recentFiles) {
            QAction *action = new QAction(QFileInfo(file).fileName(), m_mainWindow);
            action->setData(file);
            action->setToolTip(file); // Show full path on hover
            connect(action, &QAction::triggered, m_mainWindow, [this, file]() {
                this->openRecentFile(file);
            });
            m_recentFilesMenu->addAction(action);
        }
    }
}

void FileOperations::saveSession()
{
    QSettings settings("NotepadX", "Editor");
    QStringList openFiles;
    int currentIndex = m_tabWidget->currentIndex();
    
    settings.beginWriteArray("openFiles");
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        EditorWidget *editor = qobject_cast<EditorWidget *>(m_tabWidget->widget(i));
        if (editor) {
            settings.setArrayIndex(i);
            QString filePath = editor->currentFile();
            settings.setValue("filePath", filePath);
            settings.setValue("isUntitled", filePath.isEmpty());
            if (filePath.isEmpty()) {
                // For untitled files, save content to restore
                settings.setValue("content", editor->editor()->toPlainText());
            }
            settings.setValue("language", editor->currentLanguage());
            settings.setValue("zoomLevel", editor->getCurrentZoomLevel());
        }
    }
    settings.endArray();
    
    settings.setValue("activeTab", currentIndex);
}

void FileOperations::restoreSession()
{
    QSettings settings("NotepadX", "Editor");
    int size = settings.beginReadArray("openFiles");
    
    // Don't create default tab if we're going to restore files
    if (size > 0) {
        // Remove the default tab if it exists and is empty/untitled
        if (m_tabWidget->count() == 1) {
            EditorWidget *editor = qobject_cast<EditorWidget *>(m_tabWidget->widget(0));
            if (editor && editor->isUntitled() && !editor->isModified()) {
                m_tabWidget->removeTab(0);
                delete editor;
            }
        }
        
        for (int i = 0; i < size; ++i) {
            settings.setArrayIndex(i);
            QString filePath = settings.value("filePath").toString();
            bool isUntitled = settings.value("isUntitled").toBool();
            QString language = settings.value("language").toString();
            int zoomLevel = settings.value("zoomLevel", 0).toInt();
            
            EditorWidget *editor = createEditor();
            
            if (!isUntitled && !filePath.isEmpty() && QFile::exists(filePath)) {
                // Load existing file
                editor->loadFile(filePath);
            } else if (isUntitled) {
                // Restore content for untitled files
                QString content = settings.value("content").toString();
                editor->editor()->setPlainText(content);
            }
            
            // Set language and zoom level
            if (!language.isEmpty()) {
                editor->setLanguage(language);
            }
            editor->setZoomLevel(zoomLevel);
            
            // Add tab with proper title
            QString tabTitle;
            if (isUntitled || filePath.isEmpty()) {
                tabTitle = QString("Untitled %1").arg(++m_untitledCount);
            } else {
                tabTitle = QFileInfo(filePath).fileName();
            }
            
            int index = m_tabWidget->addTab(editor, tabTitle);
            
            // Set up the necessary connections
            connect(editor, &EditorWidget::fileNameChanged, m_mainWindow, [=](const QString &fileName) {
                QString tabText = fileName.isEmpty() ? 
                                QString("Untitled %1").arg(m_untitledCount) : 
                                QFileInfo(fileName).fileName();
                m_tabWidget->setTabText(m_tabWidget->indexOf(editor), tabText);
                QMetaObject::invokeMethod(m_mainWindow, "updateStatusBar");
            });
            
            // Fixed connection syntax
            connect(editor, &EditorWidget::modificationChanged, m_mainWindow, &MainWindow::documentModified);
            
            connect(editor, &EditorWidget::languageChanged, m_mainWindow, [=](const QString &) {
                QMetaObject::invokeMethod(m_mainWindow, "updateLanguageMenu");
            });
            
            connect(editor, &EditorWidget::zoomLevelChanged, m_mainWindow, [=](int) {
                QMetaObject::invokeMethod(m_mainWindow, "updateStatusBar");
            });
            
            if (m_isDarkThemeActive) {
                editor->setDarkTheme();
            }
        }
        
        // Select previously active tab
        int activeTab = settings.value("activeTab", 0).toInt();
        if (activeTab >= 0 && activeTab < m_tabWidget->count()) {
            m_tabWidget->setCurrentIndex(activeTab);
        }
    }
    settings.endArray();
    
    QMetaObject::invokeMethod(m_mainWindow, "connectEditorSignals");
    QMetaObject::invokeMethod(m_mainWindow, "updateLanguageMenu");
    QMetaObject::invokeMethod(m_mainWindow, "updateStatusBar");
}