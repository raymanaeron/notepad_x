#include "mainwindow.h"
#include "editorwidget.h"
#include "highlighting/highlighterfactory.h"
#include "findreplacedialog.h"
#include "gotolinedialog.h"
#include "codeeditor.h"   // Add this to include the CodeEditor class definition
#include <QPlainTextEdit> // Add this to include QPlainTextEdit
#include <QShortcut>
#include <QTabBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QStatusBar>
#include <QToolBar>
#include <QLabel>
#include <QStyle> // Add this to include QStyle
#include <QSettings>
#include <QStandardPaths>
#include "fonticon.h"        // Add this to include FontIcon
#include "svgiconprovider.h" // Add SVG icon provider

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), untitledCount(0), isDarkThemeActive(false),
      findReplaceDialog(nullptr), goToLineDialog(nullptr), currentZoomLevel(0)
{
    setWindowTitle("Notepad X");

    // Create tab widget first
    tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    setCentralWidget(tabWidget);

    // Connect the tabCloseRequested signal
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);
    connect(tabWidget, &QTabWidget::currentChanged, this, [this](int index)
            {
        updateTabText(index);
        updateStatusBar();
        
        // Connect to the new editor's cursor position changed signal
        connectEditorSignals(); });

    // Create menus, toolbars and status bar
    createMenus();
    createToolBar();
    createStatusBar();

    // Now read settings after all UI components are created
    readSettings();

    // Create initial tab if no files were restored
    if (tabWidget->count() == 0)
    {
        createNewTab();
    }
    
    // Make sure theme is consistently applied after all UI is created
    if (isDarkThemeActive) {
        // Re-apply dark theme to ensure consistent state across all components
        applyDarkTheme();
    }
}

MainWindow::~MainWindow()
{
    // Clean up dialogs
    if (findReplaceDialog)
    {
        delete findReplaceDialog;
    }

    if (goToLineDialog)
    {
        delete goToLineDialog;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSaveAll())
    {
        writeSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::createMenus()
{
    // Create File menu
    QMenu *fileMenu = menuBar()->addMenu("&File");

    // New tab action
    QAction *newTabAction = new QAction("&New", this);
    newTabAction->setShortcut(QKeySequence::New);
    fileMenu->addAction(newTabAction);
    connect(newTabAction, &QAction::triggered, this, &MainWindow::createNewTab);

    // Open file action
    QAction *openAction = new QAction("&Open...", this);
    openAction->setShortcut(QKeySequence::Open);
    fileMenu->addAction(openAction);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

    // Add Recent Files menu
    recentFilesMenu = fileMenu->addMenu("Recent &Files");

    // Clear recent files action
    QAction *clearRecentAction = new QAction("&Clear Recent Files", this);
    connect(clearRecentAction, &QAction::triggered, this, &MainWindow::clearRecentFiles);
    recentFilesMenu->addAction(clearRecentAction);
    recentFilesMenu->addSeparator();

    fileMenu->addSeparator();

    // Save file action
    QAction *saveAction = new QAction("&Save", this);
    saveAction->setShortcut(QKeySequence::Save);
    fileMenu->addAction(saveAction);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);

    // Save As file action
    QAction *saveAsAction = new QAction("Save &As...", this);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    fileMenu->addAction(saveAsAction);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveFileAs);

    // Close tab action
    QAction *closeTabAction = new QAction("&Close Tab", this);
    closeTabAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_W));
    fileMenu->addAction(closeTabAction);
    connect(closeTabAction, &QAction::triggered, this, &MainWindow::closeCurrentTab);

    // Add separator and Exit action
    fileMenu->addSeparator();

    QAction *exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_F4));
    fileMenu->addAction(exitAction);
    connect(exitAction, &QAction::triggered, this, [this]()
            {
        if (maybeSaveAll()) {
            qApp->quit();
        } });

    // Create Edit menu with clipboard operations
    QMenu *editMenu = menuBar()->addMenu("&Edit");

    // Undo action
    QAction *undoAction = new QAction("&Undo", this);
    undoAction->setShortcut(QKeySequence::Undo);
    editMenu->addAction(undoAction);
    connect(undoAction, &QAction::triggered, this, [this]()
            {
        EditorWidget *editor = currentEditor();
        if (editor) editor->undo(); });

    // Redo action
    QAction *redoAction = new QAction("&Redo", this);
    redoAction->setShortcut(QKeySequence::Redo);
    editMenu->addAction(redoAction);
    connect(redoAction, &QAction::triggered, this, [this]()
            {
        EditorWidget *editor = currentEditor();
        if (editor) editor->redo(); });

    editMenu->addSeparator();

    // Cut action
    QAction *cutAction = new QAction("Cu&t", this);
    cutAction->setShortcut(QKeySequence::Cut);
    editMenu->addAction(cutAction);
    connect(cutAction, &QAction::triggered, this, [this]()
            {
        EditorWidget *editor = currentEditor();
        if (editor) editor->cut(); });

    // Copy action
    QAction *copyAction = new QAction("&Copy", this);
    copyAction->setShortcut(QKeySequence::Copy);
    editMenu->addAction(copyAction);
    connect(copyAction, &QAction::triggered, this, [this]()
            {
        EditorWidget *editor = currentEditor();
        if (editor) editor->copy(); });

    // Paste action
    QAction *pasteAction = new QAction("&Paste", this);
    pasteAction->setShortcut(QKeySequence::Paste);
    editMenu->addAction(pasteAction);
    connect(pasteAction, &QAction::triggered, this, [this]()
            {
        EditorWidget *editor = currentEditor();
        if (editor) editor->paste(); });

    editMenu->addSeparator();

    // Select All action
    QAction *selectAllAction = new QAction("Select &All", this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    editMenu->addAction(selectAllAction);
    connect(selectAllAction, &QAction::triggered, this, [this]()
            {
        EditorWidget *editor = currentEditor();
        if (editor) editor->selectAll(); });

    // Add separator for Find/Replace and Go to Line
    editMenu->addSeparator();

    // Find and Replace action
    QAction *findReplaceAction = new QAction("&Find and Replace...", this);
    findReplaceAction->setShortcut(QKeySequence::Find);
    editMenu->addAction(findReplaceAction);
    connect(findReplaceAction, &QAction::triggered, this, &MainWindow::showFindReplaceDialog);

    // Go to Line action
    QAction *goToLineAction = new QAction("&Go to Line...", this);
    goToLineAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));
    editMenu->addAction(goToLineAction);
    connect(goToLineAction, &QAction::triggered, this, &MainWindow::showGoToLineDialog);

    // Create View menu with Theme submenu (moved before Language menu)
    QMenu *viewMenu = menuBar()->addMenu("&View");
    QMenu *themeMenu = viewMenu->addMenu("&Theme");

    // Create theme action group
    themeActionGroup = new QActionGroup(this);

    // Light theme action
    QAction *lightThemeAction = new QAction("&Light Theme", this);
    lightThemeAction->setCheckable(true);
    lightThemeAction->setChecked(!isDarkThemeActive); // Set based on the stored preference
    themeActionGroup->addAction(lightThemeAction);
    themeMenu->addAction(lightThemeAction);
    connect(lightThemeAction, &QAction::triggered, this, &MainWindow::applyLightTheme);

    // Dark theme action
    QAction *darkThemeAction = new QAction("&Dark Theme", this);
    darkThemeAction->setCheckable(true);
    darkThemeAction->setChecked(isDarkThemeActive); // Set based on the stored preference
    themeActionGroup->addAction(darkThemeAction);
    themeMenu->addAction(darkThemeAction);
    connect(darkThemeAction, &QAction::triggered, this, &MainWindow::applyDarkTheme);

    // Add zoom actions to View menu
    viewMenu->addSeparator();

    QAction *zoomInAction = new QAction("Zoom &In", this);
    zoomInAction->setShortcut(QKeySequence::ZoomIn); // This maps to Ctrl++ on Windows/Linux and Cmd++ on Mac
    viewMenu->addAction(zoomInAction);
    connect(zoomInAction, &QAction::triggered, this, &MainWindow::zoomIn);

    QAction *zoomOutAction = new QAction("Zoom &Out", this);
    zoomOutAction->setShortcut(QKeySequence::ZoomOut); // This maps to Ctrl+- on Windows/Linux and Cmd+- on Mac
    viewMenu->addAction(zoomOutAction);
    connect(zoomOutAction, &QAction::triggered, this, &MainWindow::zoomOut);

    QAction *resetZoomAction = new QAction("&Reset Zoom", this);
    resetZoomAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0)); // Ctrl+0 (using | instead of +)
    viewMenu->addAction(resetZoomAction);
    connect(resetZoomAction, &QAction::triggered, this, &MainWindow::resetZoom);

    // Create Language menu
    languageMenu = menuBar()->addMenu("&Language");
    languageActionGroup = new QActionGroup(this);
    connect(languageActionGroup, &QActionGroup::triggered, this, &MainWindow::languageSelected);

    // Populate language menu
    QStringList languages = HighlighterFactory::instance().supportedLanguages();

    for (const QString &lang : languages)
    {
        QAction *langAction = languageMenu->addAction(lang);
        langAction->setCheckable(true);

        // Default to "None" selected
        if (lang == "None")
        {
            langAction->setChecked(true);
        }

        languageActionGroup->addAction(langAction);
    }

    // Connect tab change signal to update language menu
    connect(tabWidget, &QTabWidget::currentChanged, this, &MainWindow::updateLanguageMenu);

    // Create Help menu
    QMenu *helpMenu = menuBar()->addMenu("&Help");

    // About action
    QAction *aboutAction = new QAction("&About", this);
    helpMenu->addAction(aboutAction);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);
}

void MainWindow::createToolBar()
{
    QToolBar *toolBar = addToolBar("Main Toolbar");
    toolBar->setObjectName("mainToolBar"); // Add object name for settings
    toolBar->setMovable(false);
    toolBar->setIconSize(QSize(24, 24)); // Slightly larger size for SVG icons

    // New file action
    QAction *newAction = toolBar->addAction("New");
    QIcon newIcon = SvgIconProvider::getIcon("new");
    // Fallback to Font Awesome if SVG fails
    if (newIcon.isNull())
    {
        newIcon = FontIcon::icon(FontIcon::FA_FILE);
    }
    newAction->setIcon(newIcon);
    connect(newAction, &QAction::triggered, this, &MainWindow::createNewTab);

    // Open file action
    QAction *openAction = toolBar->addAction("Open");
    QIcon openIcon = SvgIconProvider::getIcon("open");
    if (openIcon.isNull())
    {
        openIcon = FontIcon::icon(FontIcon::FA_FOLDER_OPEN);
    }
    openAction->setIcon(openIcon);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

    // Save file action
    QAction *saveAction = toolBar->addAction("Save");
    QIcon saveIcon = SvgIconProvider::getIcon("save");
    if (saveIcon.isNull())
    {
        saveIcon = FontIcon::icon(FontIcon::FA_SAVE);
    }
    saveAction->setIcon(saveIcon);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);

    toolBar->addSeparator();

    // Cut action
    QAction *cutAction = toolBar->addAction("Cut");
    QIcon cutIcon = SvgIconProvider::getIcon("cut");
    if (cutIcon.isNull())
    {
        cutIcon = FontIcon::icon(FontIcon::FA_CUT);
    }
    cutAction->setIcon(cutIcon);
    cutAction->setToolTip(QString("Cut (Ctrl+X)"));
    connect(cutAction, &QAction::triggered, this, [this]()
            {
        EditorWidget *editor = currentEditor();
        if (editor) editor->cut(); });

    // Copy action
    QAction *copyAction = toolBar->addAction("Copy");
    QIcon copyIcon = SvgIconProvider::getIcon("copy");
    if (copyIcon.isNull())
    {
        copyIcon = FontIcon::icon(FontIcon::FA_COPY);
    }
    copyAction->setIcon(copyIcon);
    copyAction->setToolTip(QString("Copy (Ctrl+C)"));
    connect(copyAction, &QAction::triggered, this, [this]()
            {
        EditorWidget *editor = currentEditor();
        if (editor) editor->copy(); });

    // Paste action
    QAction *pasteAction = toolBar->addAction("Paste");
    QIcon pasteIcon = SvgIconProvider::getIcon("paste");
    if (pasteIcon.isNull())
    {
        pasteIcon = FontIcon::icon(FontIcon::FA_PASTE);
    }
    pasteAction->setIcon(pasteIcon);
    pasteAction->setToolTip(QString("Paste (Ctrl+V)"));
    connect(pasteAction, &QAction::triggered, this, [this]()
            {
        EditorWidget *editor = currentEditor();
        if (editor) editor->paste(); });

    toolBar->addSeparator();

    // Undo action
    QAction *undoAction = toolBar->addAction("Undo");
    QIcon undoIcon = SvgIconProvider::getIcon("undo");
    if (undoIcon.isNull())
    {
        undoIcon = FontIcon::icon(FontIcon::FA_UNDO);
    }
    undoAction->setIcon(undoIcon);
    undoAction->setToolTip(QString("Undo (Ctrl+Z)"));
    connect(undoAction, &QAction::triggered, this, [this]()
            {
        EditorWidget *editor = currentEditor();
        if (editor) editor->undo(); });

    // Redo action
    QAction *redoAction = toolBar->addAction("Redo");
    QIcon redoIcon = SvgIconProvider::getIcon("redo");
    if (redoIcon.isNull())
    {
        redoIcon = FontIcon::icon(FontIcon::FA_REPEAT);
    }
    redoAction->setIcon(redoIcon);
    redoAction->setToolTip(QString("Redo (Ctrl+Y)"));
    connect(redoAction, &QAction::triggered, this, [this]()
            {
        EditorWidget *editor = currentEditor();
        if (editor) editor->redo(); });
}

void MainWindow::createStatusBar()
{
    // Status bar has already been created in constructor with statusBar()
    // Just add permanent widgets to it
    lineColumnLabel = new QLabel("Line: 1, Column: 1", this);
    lineColumnLabel->setMinimumWidth(150);
    statusBar()->addPermanentWidget(lineColumnLabel);

    zoomLabel = new QLabel("Zoom: 100%", this);
    zoomLabel->setMinimumWidth(100);
    statusBar()->addPermanentWidget(zoomLabel);

    modifiedLabel = new QLabel("", this);
    modifiedLabel->setMinimumWidth(80);
    statusBar()->addPermanentWidget(modifiedLabel);

    filenameLabel = new QLabel("", this);
    filenameLabel->setMinimumWidth(300);
    statusBar()->addPermanentWidget(filenameLabel);

    // Initial update
    updateStatusBar();
}

void MainWindow::updateStatusBar()
{
    EditorWidget *editor = currentEditor();
    if (!editor)
    {
        lineColumnLabel->setText("Line: 1, Column: 1");
        zoomLabel->setText("Zoom: 100%");
        modifiedLabel->setText("");
        filenameLabel->setText("");
        return;
    }

    // Update modified status
    if (editor->isModified())
    {
        modifiedLabel->setText("[Modified]");
    }
    else
    {
        modifiedLabel->setText("");
    }

    // Update filename or path
    QString filename = editor->currentFile();
    if (filename.isEmpty())
    {
        filenameLabel->setText(QString("Untitled %1").arg(untitledCount));
    }
    else
    {
        filenameLabel->setText(filename);
    }

    // Update zoom level
    int zoomLevel = editor->getCurrentZoomLevel();
    int zoomPercent = 100 + (zoomLevel * 10); // Assuming each zoom level is 10%
    zoomLabel->setText(QString("Zoom: %1%").arg(zoomPercent));

    // Update cursor position (from CodeEditor)
    CodeEditor *codeEditor = editor->editor();
    if (codeEditor)
    {
        QTextCursor cursor = codeEditor->textCursor();
        int line = cursor.blockNumber() + 1;    // Blocks are 0-based
        int column = cursor.columnNumber() + 1; // Columns are 0-based
        lineColumnLabel->setText(QString("Line: %1, Column: %2").arg(line).arg(column));
    }
}

void MainWindow::updateCursorPosition()
{
    EditorWidget *editor = currentEditor();
    if (!editor)
        return;

    CodeEditor *codeEditor = editor->editor();
    if (codeEditor)
    {
        QTextCursor cursor = codeEditor->textCursor();
        int line = cursor.blockNumber() + 1;    // Blocks are 0-based
        int column = cursor.columnNumber() + 1; // Columns are 0-based
        lineColumnLabel->setText(QString("Line: %1, Column: %2").arg(line).arg(column));
    }
}

void MainWindow::createNewTab()
{
    // Create a new EditorWidget
    EditorWidget *editor = new EditorWidget(this);

    QString tabName = QString("Untitled %1").arg(++untitledCount);
    int index = tabWidget->addTab(editor, tabName);

    // Connect signals
    connect(editor, &EditorWidget::fileNameChanged, this, [=](const QString &fileName)
            {
        QString tabText = fileName.isEmpty() ? 
                        QString("Untitled %1").arg(untitledCount) : 
                        QFileInfo(fileName).fileName();
        tabWidget->setTabText(tabWidget->indexOf(editor), tabText);
        
        // Update the status bar when filename changes
        if (editor == currentEditor()) {
            updateStatusBar();
        } });

    connect(editor, &EditorWidget::modificationChanged, this, &MainWindow::documentModified);

    connect(editor, &EditorWidget::languageChanged, this, [=](const QString &)
            {
        // Update language menu when the language changes
        if (editor == currentEditor()) {
            updateLanguageMenu();
        } });

    // Connect zoom level changed signal to update status bar
    connect(editor, &EditorWidget::zoomLevelChanged, this, [=](int)
            {
        if (editor == currentEditor()) {
            updateStatusBar();
        } });

    // Set the new tab as the current tab
    tabWidget->setCurrentIndex(index);
    editor->setFocus();

    // Apply the active theme to the new tab
    if (isDarkThemeActive)
    {
        editor->setDarkTheme();
    }
    else
    {
        editor->setLightTheme();
    }

    // Update the language menu to reflect the current editor
    updateLanguageMenu();

    // Connect editor signals for cursor position tracking
    connectEditorSignals();
}

void MainWindow::closeCurrentTab()
{
    int currentIndex = tabWidget->currentIndex();
    if (currentIndex != -1)
    {
        onTabCloseRequested(currentIndex);
    }
}

void MainWindow::onTabCloseRequested(int index)
{
    EditorWidget *editor = qobject_cast<EditorWidget *>(tabWidget->widget(index));
    if (!editor)
        return;

    if (editor->maybeSave())
    {
        tabWidget->removeTab(index);
        delete editor;

        // If the last tab was closed, create a new one
        if (tabWidget->count() == 0)
        {
            createNewTab();
        }
    }
}

void MainWindow::openFile()
{
    if (!ensureHasOpenTab())
        return;

    QString fileName = QFileDialog::getOpenFileName(this);
    if (fileName.isEmpty())
        return;

    if (openFileHelper(fileName))
    {
        // Add to recent files if successfully opened
        addToRecentFiles(fileName);
    }
}

bool MainWindow::openFileHelper(const QString &fileName)
{
    if (fileName.isEmpty())
        return false;

    // Check if the file is already open
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        EditorWidget *editor = qobject_cast<EditorWidget *>(tabWidget->widget(i));
        if (editor && QFileInfo(fileName) == QFileInfo(editor->currentFile()))
        {
            tabWidget->setCurrentIndex(i);
            return true;
        }
    }

    // If current tab is untitled and not modified, use it
    EditorWidget *currentEditorWidget = currentEditor();
    if (currentEditorWidget && currentEditorWidget->isUntitled() && !currentEditorWidget->isModified())
    {
        if (currentEditorWidget->loadFile(fileName))
        {
            statusBar()->showMessage(tr("File loaded"), 2000);
            return true;
        }
        return false;
    }

    // Create a new tab
    EditorWidget *editor = new EditorWidget(this);
    if (editor->loadFile(fileName))
    {
        int index = tabWidget->addTab(editor, QFileInfo(fileName).fileName());
        tabWidget->setCurrentIndex(index);

        // Connect signals
        connect(editor, &EditorWidget::fileNameChanged, this, [=](const QString &fileName)
                {
            QString tabText = fileName.isEmpty() ? 
                            QString("Untitled %1").arg(untitledCount) : 
                            QFileInfo(fileName).fileName();
            tabWidget->setTabText(tabWidget->indexOf(editor), tabText);
            
            // Update status bar if this is the current editor
            if (editor == currentEditor()) {
                updateStatusBar();
            } });

        connect(editor, &EditorWidget::modificationChanged, this, &MainWindow::documentModified);

        // Connect zoom level changed signal to update status bar
        connect(editor, &EditorWidget::zoomLevelChanged, this, [=](int)
                {
            if (editor == currentEditor()) {
                updateStatusBar();
            } });

        statusBar()->showMessage(tr("File loaded"), 2000);

        // Apply the active theme to this newly opened file
        if (isDarkThemeActive)
        {
            editor->setDarkTheme();
        }
        else
        {
            editor->setLightTheme();
        }

        // Connect editor signals for cursor position tracking
        connectEditorSignals();

        return true;
    }
    else
    {
        delete editor;
        return false;
    }
}

bool MainWindow::saveFile()
{
    if (!ensureHasOpenTab())
        return false;

    EditorWidget *editor = currentEditor();
    if (!editor)
        return false;

    if (editor->save())
    {
        statusBar()->showMessage(tr("File saved"), 2000);
        // Add to recent files when saving a new file
        if (!editor->currentFile().isEmpty())
        {
            addToRecentFiles(editor->currentFile());
        }
        return true;
    }

    return false;
}

bool MainWindow::saveFileAs()
{
    if (!ensureHasOpenTab())
        return false;

    EditorWidget *editor = currentEditor();
    if (!editor)
        return false;

    if (editor->saveAs())
    {
        statusBar()->showMessage(tr("File saved"), 2000);
        // Add to recent files when saving with a new name
        if (!editor->currentFile().isEmpty())
        {
            addToRecentFiles(editor->currentFile());
        }
        return true;
    }

    return false;
}

EditorWidget *MainWindow::currentEditor()
{
    return qobject_cast<EditorWidget *>(tabWidget->currentWidget());
}

bool MainWindow::ensureHasOpenTab()
{
    if (tabWidget->count() == 0)
    {
        createNewTab();
    }
    return tabWidget->count() > 0;
}

void MainWindow::updateTabText(int index)
{
    if (index < 0)
        return;

    EditorWidget *editor = qobject_cast<EditorWidget *>(tabWidget->widget(index));
    if (!editor)
        return;

    // Update window title to show current file
    if (index == tabWidget->currentIndex())
    {
        QString title = editor->currentFile();
        if (title.isEmpty())
            title = QString("Untitled %1").arg(untitledCount);
        else
            title = QFileInfo(title).fileName();

        if (editor->isModified())
            title += " *";

        title += " - Notepad X";
        setWindowTitle(title);
    }
}

void MainWindow::documentModified(bool modified)
{
    EditorWidget *editor = qobject_cast<EditorWidget *>(sender());
    if (!editor)
        return;

    int index = tabWidget->indexOf(editor);
    if (index < 0)
        return;

    QString tabText = tabWidget->tabText(index);
    if (modified && !tabText.endsWith(" *"))
    {
        tabWidget->setTabText(index, tabText + " *");
    }
    else if (!modified && tabText.endsWith(" *"))
    {
        tabWidget->setTabText(index, tabText.left(tabText.length() - 2));
    }

    // Also update window title if this is the current tab
    if (index == tabWidget->currentIndex())
    {
        updateTabText(index);

        // Update the status bar to show modified status
        updateStatusBar();
    }
}

bool MainWindow::maybeSaveAll()
{
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        EditorWidget *editor = qobject_cast<EditorWidget *>(tabWidget->widget(i));
        if (editor && editor->isModified())
        {
            tabWidget->setCurrentIndex(i);
            if (!editor->maybeSave())
            {
                return false;
            }
        }
    }
    return true;
}

void MainWindow::languageSelected(QAction *action)
{
    EditorWidget *editor = currentEditor();
    if (editor)
    {
        editor->setLanguage(action->text());
    }
}

void MainWindow::updateLanguageMenu()
{
    EditorWidget *editor = currentEditor();
    if (editor)
    {
        QString currentLang = editor->currentLanguage();

        // Find and check the appropriate action
        for (QAction *action : languageActionGroup->actions())
        {
            if (action->text() == currentLang)
            {
                action->setChecked(true);
                break;
            }
        }
    }
}

void MainWindow::applyLightTheme()
{
    // Set light theme for the application
    qApp->setStyle("Fusion");

    // Create and apply VS Code-like light theme palette
    QPalette lightPalette;
    lightPalette.setColor(QPalette::Window, QColor(240, 240, 240));        // VS Code light window background
    lightPalette.setColor(QPalette::WindowText, QColor(0, 0, 0));          // VS Code light text color
    lightPalette.setColor(QPalette::Base, QColor(255, 255, 255));          // VS Code editor background (white)
    lightPalette.setColor(QPalette::AlternateBase, QColor(245, 245, 245)); // Slightly darker
    lightPalette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));   // Tooltip background
    lightPalette.setColor(QPalette::ToolTipText, QColor(0, 0, 0));         // Tooltip text
    lightPalette.setColor(QPalette::Text, QColor(0, 0, 0));                // Default text color
    lightPalette.setColor(QPalette::Button, QColor(240, 240, 240));        // Button background
    lightPalette.setColor(QPalette::ButtonText, QColor(0, 0, 0));          // Button text
    lightPalette.setColor(QPalette::BrightText, QColor(0, 0, 0));          // Bright text
    lightPalette.setColor(QPalette::Link, QColor(0, 122, 204));            // VS Code blue
    lightPalette.setColor(QPalette::Highlight, QColor(185, 215, 255));     // VS Code light blue selection
    lightPalette.setColor(QPalette::HighlightedText, QColor(0, 0, 0));     // Text on selection

    qApp->setPalette(lightPalette);

    // Update theme tracking state
    isDarkThemeActive = false;

    // Update all existing editors
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        EditorWidget *editor = qobject_cast<EditorWidget *>(tabWidget->widget(i));
        if (editor)
        {
            // Update editor theme
            editor->setLightTheme();
        }
    }

    // Create custom icon style to ensure toolbar icons are visible in light theme
    QList<QAction *> actions = findChildren<QAction *>();
    for (QAction *action : actions)
    {
        if (action->icon().isNull())
            continue;
            
        // Create a new icon with custom color overlay for light theme
        QIcon originalIcon = action->icon();
        QIcon::Mode mode = QIcon::Normal;
        
        // Get all available sizes of the icon
        QList<QSize> sizes = originalIcon.availableSizes(mode);
        if (sizes.isEmpty())
            sizes.append(QSize(24, 24)); // Default size
            
        QIcon newIcon;
        for (const QSize &size : sizes)
        {
            // Get the pixmap from the original icon
            QPixmap pixmap = originalIcon.pixmap(size);
            
            // Create a darker version of the icon for light theme
            QPainter painter(&pixmap);
            painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            painter.fillRect(pixmap.rect(), QColor(50, 50, 50));  // Dark color for light background
            painter.end();
            
            newIcon.addPixmap(pixmap, mode);
        }
        
        action->setIcon(newIcon);
    }

    // Save the theme preference immediately
    QSettings settings("NotepadX", "Editor");
    settings.setValue("darkTheme", isDarkThemeActive);
}

void MainWindow::applyDarkTheme()
{
    // Set dark theme for the application
    qApp->setStyle("Fusion");

    // Create and apply VS Code-like dark theme palette
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(30, 30, 30));             // VS Code background
    darkPalette.setColor(QPalette::WindowText, QColor(220, 220, 220));      // VS Code text
    darkPalette.setColor(QPalette::Base, QColor(30, 30, 30));               // VS Code editor background
    darkPalette.setColor(QPalette::AlternateBase, QColor(45, 45, 45));      // Slightly lighter
    darkPalette.setColor(QPalette::ToolTipBase, QColor(30, 30, 30));        // Tooltip background
    darkPalette.setColor(QPalette::ToolTipText, QColor(220, 220, 220));     // Tooltip text
    darkPalette.setColor(QPalette::Text, QColor(220, 220, 220));            // Text color
    darkPalette.setColor(QPalette::Button, QColor(45, 45, 45));             // Button background
    darkPalette.setColor(QPalette::ButtonText, QColor(220, 220, 220));      // Button text
    darkPalette.setColor(QPalette::BrightText, QColor(255, 255, 255));      // Bright text
    darkPalette.setColor(QPalette::Link, QColor(0, 122, 204));              // VS Code blue
    darkPalette.setColor(QPalette::Highlight, QColor(0, 122, 204));         // VS Code selection blue
    darkPalette.setColor(QPalette::HighlightedText, QColor(255, 255, 255)); // White text on selection

    qApp->setPalette(darkPalette);

    // Update theme tracking state
    isDarkThemeActive = true;

    // Update all existing editors
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        EditorWidget *editor = qobject_cast<EditorWidget *>(tabWidget->widget(i));
        if (editor)
        {
            // Update editor theme
            editor->setDarkTheme();
        }
    }

    // Create custom icon style to ensure toolbar icons are visible in dark theme
    QList<QAction *> actions = findChildren<QAction *>();
    for (QAction *action : actions)
    {
        if (action->icon().isNull())
            continue;
            
        // Create a new icon with custom color overlay for dark theme
        QIcon originalIcon = action->icon();
        QIcon::Mode mode = QIcon::Normal;
        
        // Get all available sizes of the icon
        QList<QSize> sizes = originalIcon.availableSizes(mode);
        if (sizes.isEmpty())
            sizes.append(QSize(24, 24)); // Default size
            
        QIcon newIcon;
        for (const QSize &size : sizes)
        {
            // Get the pixmap from the original icon
            QPixmap pixmap = originalIcon.pixmap(size);
            
            // Create a bright version of the icon for dark theme
            QPainter painter(&pixmap);
            painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            painter.fillRect(pixmap.rect(), QColor(220, 220, 220));  // Light color for dark background
            painter.end();
            
            newIcon.addPixmap(pixmap, mode);
        }
        
        action->setIcon(newIcon);
    }

    // Save the theme preference immediately
    QSettings settings("NotepadX", "Editor");
    settings.setValue("darkTheme", isDarkThemeActive);
}

void MainWindow::showFindReplaceDialog()
{
    if (!ensureHasOpenTab())
        return;

    EditorWidget *editor = currentEditor();
    if (!editor)
        return;

    // Create the dialog if it doesn't exist
    if (!findReplaceDialog)
    {
        findReplaceDialog = new FindReplaceDialog(this);
    }

    // CodeEditor inherits from QPlainTextEdit, so we can use static_cast safely
    // or just pass it directly since it IS a QPlainTextEdit
    findReplaceDialog->setEditor(editor->editor());

    // Show the dialog
    findReplaceDialog->show();
    findReplaceDialog->raise();
    findReplaceDialog->activateWindow();
}

void MainWindow::showGoToLineDialog()
{
    if (!ensureHasOpenTab())
        return;

    EditorWidget *editor = currentEditor();
    if (!editor)
        return;

    // Create the dialog if it doesn't exist
    if (!goToLineDialog)
    {
        goToLineDialog = new GoToLineDialog(this);
    }

    // CodeEditor inherits from QPlainTextEdit, so we can use static_cast safely
    // or just pass it directly since it IS a QPlainTextEdit
    goToLineDialog->setEditor(editor->editor());

    // Show the dialog
    goToLineDialog->show();
    goToLineDialog->raise();
    goToLineDialog->activateWindow();
}

void MainWindow::connectEditorSignals()
{
    EditorWidget *editor = currentEditor();
    if (!editor)
        return;

    // Connect the editor's cursor position changed signal
    CodeEditor *codeEditor = editor->editor();
    if (codeEditor)
    {
        // Disconnect previous connections if any
        disconnect(codeEditor, &CodeEditor::cursorPositionChanged, this, nullptr);

        // Connect to this editor's cursor position changed signal
        connect(codeEditor, &CodeEditor::cursorPositionChanged, this, &MainWindow::updateCursorPosition);

        // Connect to zoom level changes directly from CodeEditor
        disconnect(codeEditor, &CodeEditor::zoomLevelChanged, this, nullptr);
        connect(codeEditor, &CodeEditor::zoomLevelChanged, this, [this](int)
                { updateStatusBar(); });
    }

    // Also connect to EditorWidget's zoomLevelChanged signal
    disconnect(editor, &EditorWidget::zoomLevelChanged, this, nullptr);
    connect(editor, &EditorWidget::zoomLevelChanged, this, [this](int)
            { updateStatusBar(); });
}

void MainWindow::readSettings()
{
    QSettings settings("NotepadX", "Editor");

    // Restore window geometry
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (!geometry.isEmpty())
    {
        restoreGeometry(geometry);
    }
    else
    {
        // Default size if no settings exist
        resize(800, 600);
    }

    // Restore window state (toolbars, docks, etc.)
    const QByteArray state = settings.value("windowState", QByteArray()).toByteArray();
    if (!state.isEmpty())
    {
        restoreState(state);
    }

    // Restore theme setting - but don't apply yet
    isDarkThemeActive = settings.value("darkTheme", false).toBool();

    // Update theme menu items to match the loaded preference
    if (themeActionGroup)
    {
        for (QAction *action : themeActionGroup->actions())
        {
            if ((action->text() == "&Light Theme" && !isDarkThemeActive) ||
                (action->text() == "&Dark Theme" && isDarkThemeActive))
            {
                action->setChecked(true);
                break;
            }
        }
    }

    // Restore recent files list with validation
    recentFiles = settings.value("recentFiles").toStringList();

    // Remove any non-existent files from the recent files list
    QStringList validRecentFiles;
    foreach (const QString &filePath, recentFiles)
    {
        if (QFile::exists(filePath))
        {
            validRecentFiles.append(filePath);
        }
    }
    recentFiles = validRecentFiles;

    // Only restore session files if that setting is enabled
    bool restoreSession = settings.value("restoreSession", true).toBool();
    if (restoreSession)
    {
        try
        {
            // Restore last open files
            int fileCount = settings.beginReadArray("openFiles");
            int currentTabIndex = settings.value("currentTabIndex", 0).toInt();

            for (int i = 0; i < fileCount; ++i)
            {
                settings.setArrayIndex(i);
                QString filePath = settings.value("path").toString();

                if (!filePath.isEmpty() && QFile::exists(filePath))
                {
                    EditorWidget *editor = new EditorWidget(this);

                    try
                    {
                        if (editor->loadFile(filePath))
                        {
                            int index = tabWidget->addTab(editor, QFileInfo(filePath).fileName());

                            // Connect signals
                            connect(editor, &EditorWidget::fileNameChanged, this, [=](const QString &fileName)
                                    {
                                QString tabText = fileName.isEmpty() ? 
                                                QString("Untitled %1").arg(untitledCount) : 
                                                QFileInfo(fileName).fileName();
                                tabWidget->setTabText(tabWidget->indexOf(editor), tabText);
                                
                                // Update status bar if this is the current editor
                                if (editor == currentEditor()) {
                                    updateStatusBar();
                                } });

                            connect(editor, &EditorWidget::modificationChanged, this, &MainWindow::documentModified);
                            connect(editor, &EditorWidget::languageChanged, this, [=](const QString &)
                                    {
                                // Update language menu when the language changes
                                if (editor == currentEditor()) {
                                    updateLanguageMenu();
                                } });
                        }
                        else
                        {
                            delete editor;
                        }
                    }
                    catch (...)
                    {
                        // Safety fallback if loading fails
                        delete editor;
                    }
                }
            }
            settings.endArray();

            // Set the current tab safely
            if (currentTabIndex >= 0 && currentTabIndex < tabWidget->count())
            {
                tabWidget->setCurrentIndex(currentTabIndex);
            }
        }
        catch (...)
        {
            // If anything goes wrong during session restore, just continue
            qDebug("Error restoring session");
        }
    }

    // Apply the appropriate theme AFTER all UI elements are created
    if (isDarkThemeActive)
    {
        applyDarkTheme();
    }
    else
    {
        applyLightTheme();
    }

    // Update the recent files menu
    updateRecentFilesMenu();
}

void MainWindow::writeSettings()
{
    QSettings settings("NotepadX", "Editor");

    // Save window geometry
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    // Save theme setting
    settings.setValue("darkTheme", isDarkThemeActive);

    // Save recent files
    settings.setValue("recentFiles", recentFiles);

    // Save open files
    int validFileCount = 0;
    settings.beginWriteArray("openFiles");

    for (int i = 0; i < tabWidget->count(); ++i)
    {
        EditorWidget *editor = qobject_cast<EditorWidget *>(tabWidget->widget(i));
        if (editor && !editor->isUntitled())
        {
            QString filePath = editor->currentFile();
            if (!filePath.isEmpty() && QFile::exists(filePath))
            {
                settings.setArrayIndex(validFileCount++);
                settings.setValue("path", filePath);
            }
        }
    }
    settings.endArray();

    // Save current tab index
    settings.setValue("currentTabIndex", tabWidget->currentIndex());
    
    // Save restore session setting (always true for now)
    settings.setValue("restoreSession", true);
}

void MainWindow::addToRecentFiles(const QString &filePath)
{
    // Remove the file path if it already exists in the list
    recentFiles.removeAll(filePath);

    // Add the file path to the beginning of the list
    recentFiles.prepend(filePath);

    // Keep the list size limited to maxRecentFiles
    while (recentFiles.size() > maxRecentFiles)
    {
        recentFiles.removeLast();
    }

    // Update the menu
    updateRecentFilesMenu();
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString fileName = action->data().toString();

        if (QFile::exists(fileName))
        {
            if (openFileHelper(fileName))
            {
                // Move to top of recent files list
                addToRecentFiles(fileName);
            }
        }
        else
        {
            // File not found, show error message
            QMessageBox::warning(this, "File Not Found",
                                 QString("The file '%1' could not be found.").arg(fileName));

            // Remove from recent files list
            recentFiles.removeAll(fileName);
            updateRecentFilesMenu();
        }
    }
}

void MainWindow::clearRecentFiles()
{
    recentFiles.clear();
    updateRecentFilesMenu();
}

void MainWindow::updateRecentFilesMenu()
{
    if (!recentFilesMenu)
    {
        return; // Safety check in case the menu hasn't been created yet
    }

    // Clear existing recent file actions, preserving the clear action and separator
    QList<QAction *> actions = recentFilesMenu->actions();
    if (actions.size() >= 2)
    {
        // Keep the first two actions (Clear Recent Files action and separator)
        QAction *clearAction = actions.at(0);
        QAction *separator = actions.at(1);

        // Remove all other actions
        for (int i = 2; i < actions.size(); ++i)
        {
            recentFilesMenu->removeAction(actions[i]);
        }

        // Update the clear action enabled state
        clearAction->setEnabled(!recentFiles.isEmpty());
    }
    else
    {
        // If the menu wasn't set up properly, recreate it
        recentFilesMenu->clear();

        QAction *clearRecentAction = new QAction("&Clear Recent Files", this);
        connect(clearRecentAction, &QAction::triggered, this, &MainWindow::clearRecentFiles);
        recentFilesMenu->addAction(clearRecentAction);
        clearRecentAction->setEnabled(!recentFiles.isEmpty());

        recentFilesMenu->addSeparator();
    }

    // Add actions for recent files
    for (const QString &file : recentFiles)
    {
        QAction *action = new QAction(QFileInfo(file).fileName(), this);
        action->setData(file);
        action->setStatusTip(file);
        connect(action, &QAction::triggered, this, &MainWindow::openRecentFile);
        recentFilesMenu->addAction(action);
    }
}

void MainWindow::zoomIn()
{
    EditorWidget *editor = currentEditor();
    if (editor)
    {
        editor->zoomIn();
        updateStatusBar();
    }
}

void MainWindow::zoomOut()
{
    EditorWidget *editor = currentEditor();
    if (editor)
    {
        editor->zoomOut();
        updateStatusBar();
    }
}

void MainWindow::resetZoom()
{
    EditorWidget *editor = currentEditor();
    if (editor)
    {
        editor->resetZoom();
        updateStatusBar();
    }
}

void MainWindow::showAboutDialog()
{
    // Create a QMessageBox with rich text support
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("About NotepadX");
    
    // Create HTML content with styled title
    QString htmlMessage = "<html><body>"
                          "<h2 style='margin-top:0;'><b>NotepadX</b></h2>"
                          "<p>A cross platform text editor</p>"
                          "<p>Copyright (c) 2025 by Rayman Aeron</p>"
                          "<p>All rights reserved.</p>"
                          "<p>This program is free software: you can redistribute it and/or modify<br>"
                          "it under the terms of the GNU General Public License as published by<br>"
                          "the Free Software Foundation, either version 3 of the License, or<br>"
                          "(at your option) any later version.</p>"
                          "<p>This program is distributed in the hope that it will be useful,<br>"
                          "but WITHOUT ANY WARRANTY; without even the implied warranty of<br>"
                          "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the<br>"
                          "GNU General Public License for more details.</p>"
                          "<p>You should have received a copy of the GNU General Public License<br>"
                          "along with this program. If not, see &lt;http://www.gnu.org/licenses/&gt;.</p>"
                          "</body></html>";
    
    // Set the text as rich text (HTML)
    msgBox.setText(htmlMessage);
    msgBox.setTextFormat(Qt::RichText);
    
    // Display the dialog
    msgBox.exec();
}
