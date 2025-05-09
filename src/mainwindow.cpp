#include "mainwindow.h"
#include "editorwidget.h"
#include "highlighting/highlighterfactory.h"
#include "findreplacedialog.h"
#include "gotolinedialog.h"
#include "codeeditor.h"
#include "fileoperations.h"
#include "editormanager.h"
#include "searchmanager.h"
#include <QPlainTextEdit>
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
#include <QStyle>
#include <QSettings>
#include <QStandardPaths>
#include <QDebug>
#include <QFile>
#include "fonticon.h"
#include "svgiconprovider.h"

// Debug helper function - make it static to avoid multiple definition error
static void debugLogMessage(const QString& message) {
    qDebug() << message;
    QFile file("crash_log.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream stream(&file);
        stream << message << "\n";
        file.close();
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    debugLogMessage("MainWindow constructor start");
    
    setWindowTitle("NotepadX");

    try {
        // Initialize member pointers to nullptr to prevent crashes
        fileOps = nullptr;
        editorMgr = nullptr;
        searchMgr = nullptr;

        // Create the UI components first
        debugLogMessage("Creating tab widget");
        createTabWidget();
        debugLogMessage("Tab widget created");
        
        // Create menus before status bar and managers
        debugLogMessage("Creating menus");
        createMenus();
        debugLogMessage("Menus created");
        
        debugLogMessage("Creating toolbar");
        createToolBar();
        debugLogMessage("Toolbar created");
        
        debugLogMessage("Creating status bar");
        createStatusBar();
        debugLogMessage("Status bar created");

        // Create module managers after creating UI elements
        debugLogMessage("Creating file operations manager");
        fileOps = new FileOperations(this);
        debugLogMessage("File operations manager created");
        
        debugLogMessage("Creating editor manager");
        editorMgr = new EditorManager(this);
        debugLogMessage("Editor manager created");
        
        debugLogMessage("Creating search manager");
        searchMgr = new SearchManager(this);
        debugLogMessage("Search manager created");
        
        // Load settings
        debugLogMessage("Reading settings");
        readSettings();
        debugLogMessage("Settings read");
        
        // Only create a new tab if no tabs were restored from session
        if (tabWidget->count() == 0)
        {
            debugLogMessage("Creating new tab");
            createNewTab();
            debugLogMessage("New tab created");
        }

        if (editorMgr && editorMgr->isDarkTheme())
        {
            debugLogMessage("Applying dark theme");
            applyDarkTheme();
            debugLogMessage("Dark theme applied");
        }
        
        debugLogMessage("MainWindow constructor completed successfully");
    }
    catch (const std::exception& e) {
        debugLogMessage(QString("Exception in MainWindow constructor: %1").arg(e.what()));
        throw; // Rethrow to be caught by main
    }
    catch (...) {
        debugLogMessage("Unknown exception in MainWindow constructor");
        throw; // Rethrow to be caught by main
    }
}

MainWindow::~MainWindow()
{
    // Module managers will clean up their own resources
    delete fileOps;
    delete editorMgr;
    delete searchMgr;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSaveAll())
    {
        fileOps->saveSession();
        writeSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::createTabWidget()
{
    tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    
    // Ensure tab bar is explicitly left aligned
    tabWidget->tabBar()->setExpanding(false);
    tabWidget->tabBar()->setDrawBase(true);
    tabWidget->tabBar()->setElideMode(Qt::ElideRight);
    
    setCentralWidget(tabWidget);
    tabWidget->tabBar()->installEventFilter(this);

    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);
    connect(tabWidget, &QTabWidget::currentChanged, this, [this](int index)
            {
        updateTabText(index);
        updateStatusBar();
        connectEditorSignals(); });
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu("&File");

    QAction *newTabAction = new QAction("&New", this);
    newTabAction->setShortcut(QKeySequence::New);
    fileMenu->addAction(newTabAction);
    connect(newTabAction, &QAction::triggered, this, &MainWindow::createNewTab);

    QAction *openAction = new QAction("&Open...", this);
    openAction->setShortcut(QKeySequence::Open);
    fileMenu->addAction(openAction);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

    QMenu *recentFilesMenu = fileMenu->addMenu("Recent &Files");

    QAction *clearRecentAction = new QAction("&Clear Recent Files", this);
    connect(clearRecentAction, &QAction::triggered, this, &MainWindow::clearRecentFiles);
    recentFilesMenu->addAction(clearRecentAction);
    recentFilesMenu->addSeparator();

    fileMenu->addSeparator();

    QAction *saveAction = new QAction("&Save", this);
    saveAction->setShortcut(QKeySequence::Save);
    fileMenu->addAction(saveAction);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);

    QAction *saveAsAction = new QAction("Save &As...", this);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    fileMenu->addAction(saveAsAction);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveFileAs);

    QAction *closeTabAction = new QAction("&Close Tab", this);
    closeTabAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_W));
    fileMenu->addAction(closeTabAction);
    connect(closeTabAction, &QAction::triggered, this, &MainWindow::closeCurrentTab);

    fileMenu->addSeparator();

    QAction *exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_F4));
    fileMenu->addAction(exitAction);
    connect(exitAction, &QAction::triggered, this, [this]()
            {
        if (maybeSaveAll()) {
            qApp->quit();
        } });

    QMenu *editMenu = menuBar()->addMenu("&Edit");

    QAction *undoAction = new QAction("&Undo", this);
    undoAction->setShortcut(QKeySequence::Undo);
    editMenu->addAction(undoAction);
    connect(undoAction, &QAction::triggered, this, [this]()
            {
        // We'll connect this properly after editor manager is created
        if (editorMgr) {
            EditorWidget *editor = editorMgr->currentEditor();
            if (editor) editor->undo(); 
        }
    });

    QAction *redoAction = new QAction("&Redo", this);
    redoAction->setShortcut(QKeySequence::Redo);
    editMenu->addAction(redoAction);
    connect(redoAction, &QAction::triggered, this, [this]()
            {
        if (editorMgr) {
            EditorWidget *editor = editorMgr->currentEditor();
            if (editor) editor->redo(); 
        }
    });

    editMenu->addSeparator();

    QAction *cutAction = new QAction("Cu&t", this);
    cutAction->setShortcut(QKeySequence::Cut);
    editMenu->addAction(cutAction);
    connect(cutAction, &QAction::triggered, this, [this]()
            {
        if (editorMgr) {
            EditorWidget *editor = editorMgr->currentEditor();
            if (editor) editor->cut();
        }
    });

    QAction *copyAction = new QAction("&Copy", this);
    copyAction->setShortcut(QKeySequence::Copy);
    editMenu->addAction(copyAction);
    connect(copyAction, &QAction::triggered, this, [this]()
            {
        if (editorMgr) {
            EditorWidget *editor = editorMgr->currentEditor();
            if (editor) editor->copy(); 
        }
    });

    QAction *pasteAction = new QAction("&Paste", this);
    pasteAction->setShortcut(QKeySequence::Paste);
    editMenu->addAction(pasteAction);
    connect(pasteAction, &QAction::triggered, this, [this]()
            {
        if (editorMgr) {
            EditorWidget *editor = editorMgr->currentEditor();
            if (editor) editor->paste(); 
        }
    });

    editMenu->addSeparator();

    QAction *selectAllAction = new QAction("Select &All", this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    editMenu->addAction(selectAllAction);
    connect(selectAllAction, &QAction::triggered, this, [this]()
            {
        if (editorMgr) {
            EditorWidget *editor = editorMgr->currentEditor();
            if (editor) editor->selectAll(); 
        }
    });

    editMenu->addSeparator();

    QAction *findReplaceAction = new QAction("&Find and Replace...", this);
    findReplaceAction->setShortcut(QKeySequence::Find);
    editMenu->addAction(findReplaceAction);
    connect(findReplaceAction, &QAction::triggered, this, &MainWindow::showFindReplaceDialog);

    QAction *goToLineAction = new QAction("&Go to Line...", this);
    goToLineAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));
    editMenu->addAction(goToLineAction);
    connect(goToLineAction, &QAction::triggered, this, &MainWindow::showGoToLineDialog);

    QMenu *viewMenu = menuBar()->addMenu("&View");
    QMenu *themeMenu = viewMenu->addMenu("&Theme");

    QActionGroup *themeActionGroup = new QActionGroup(this);

    QAction *lightThemeAction = new QAction("&Light Theme", this);
    lightThemeAction->setCheckable(true);
    
    // Default to light theme - will be updated later
    lightThemeAction->setChecked(true);
    
    themeActionGroup->addAction(lightThemeAction);
    themeMenu->addAction(lightThemeAction);
    connect(lightThemeAction, &QAction::triggered, this, &MainWindow::applyLightTheme);

    QAction *darkThemeAction = new QAction("&Dark Theme", this);
    darkThemeAction->setCheckable(true);
    darkThemeAction->setChecked(false); // Default to light theme
    themeActionGroup->addAction(darkThemeAction);
    themeMenu->addAction(darkThemeAction);
    connect(darkThemeAction, &QAction::triggered, this, &MainWindow::applyDarkTheme);

    viewMenu->addSeparator();

    QAction *zoomInAction = new QAction("Zoom &In", this);
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    viewMenu->addAction(zoomInAction);
    connect(zoomInAction, &QAction::triggered, this, &MainWindow::zoomIn);

    QAction *zoomOutAction = new QAction("Zoom &Out", this);
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    viewMenu->addAction(zoomOutAction);
    connect(zoomOutAction, &QAction::triggered, this, &MainWindow::zoomOut);

    QAction *resetZoomAction = new QAction("&Reset Zoom", this);
    resetZoomAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    viewMenu->addAction(resetZoomAction);
    connect(resetZoomAction, &QAction::triggered, this, &MainWindow::resetZoom);

    viewMenu->addSeparator();

    QAction *wordWrapAction = new QAction("&Word Wrap", this);
    wordWrapAction->setCheckable(true);
    wordWrapAction->setChecked(false); // Default setting, will be updated later
    viewMenu->addAction(wordWrapAction);
    connect(wordWrapAction, &QAction::triggered, this, &MainWindow::toggleWordWrap);

    QMenu *languageMenu = menuBar()->addMenu("&Language");
    QActionGroup *languageActionGroup = new QActionGroup(this);
    connect(languageActionGroup, &QActionGroup::triggered, this, &MainWindow::languageSelected);

    // Get languages from the factory
    QStringList languages = HighlighterFactory::instance().supportedLanguages();

    for (const QString &lang : languages)
    {
        QAction *langAction = languageMenu->addAction(lang);
        langAction->setCheckable(true);

        if (lang == "None")
        {
            langAction->setChecked(true);
        }

        languageActionGroup->addAction(langAction);
    }

    connect(tabWidget, &QTabWidget::currentChanged, this, &MainWindow::updateLanguageMenu);

    QMenu *helpMenu = menuBar()->addMenu("&Help");

    QAction *aboutAction = new QAction("&About", this);
    helpMenu->addAction(aboutAction);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);
}

void MainWindow::createToolBar()
{
    QToolBar *toolBar = addToolBar("Main Toolbar");
    toolBar->setObjectName("mainToolBar");
    toolBar->setMovable(false);
    toolBar->setIconSize(QSize(24, 24));

    QAction *newAction = toolBar->addAction("New");
    QIcon newIcon = SvgIconProvider::getIcon("new");
    if (newIcon.isNull())
    {
        newIcon = FontIcon::icon(FontIcon::FA_FILE);
    }
    newAction->setIcon(newIcon);
    connect(newAction, &QAction::triggered, this, &MainWindow::createNewTab);

    QAction *openAction = toolBar->addAction("Open");
    QIcon openIcon = SvgIconProvider::getIcon("open");
    if (openIcon.isNull())
    {
        openIcon = FontIcon::icon(FontIcon::FA_FOLDER_OPEN);
    }
    openAction->setIcon(openIcon);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

    QAction *saveAction = toolBar->addAction("Save");
    QIcon saveIcon = SvgIconProvider::getIcon("save");
    if (saveIcon.isNull())
    {
        saveIcon = FontIcon::icon(FontIcon::FA_SAVE);
    }
    saveAction->setIcon(saveIcon);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);

    toolBar->addSeparator();

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
        if (editorMgr) {
            EditorWidget *editor = editorMgr->currentEditor();
            if (editor) editor->cut(); 
        }
    });

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
        if (editorMgr) {
            EditorWidget *editor = editorMgr->currentEditor();
            if (editor) editor->copy(); 
        }
    });

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
        if (editorMgr) {
            EditorWidget *editor = editorMgr->currentEditor();
            if (editor) editor->paste(); 
        }
    });

    toolBar->addSeparator();

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
        if (editorMgr) {
            EditorWidget *editor = editorMgr->currentEditor();
            if (editor) editor->undo(); 
        }
    });

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
        if (editorMgr) {
            EditorWidget *editor = editorMgr->currentEditor();
            if (editor) editor->redo(); 
        }
    });
}

void MainWindow::createStatusBar()
{
    try {
        debugLogMessage("Creating status bar labels");
        
        // Store these as class members for easier access by modules
        lineColumnLabel = new QLabel("Line: 1, Column: 1", this);
        lineColumnLabel->setMinimumWidth(150);
        statusBar()->addPermanentWidget(lineColumnLabel);
        debugLogMessage("Line column label created");

        zoomLabel = new QLabel("Zoom: 100%", this);
        zoomLabel->setMinimumWidth(100);
        statusBar()->addPermanentWidget(zoomLabel);
        debugLogMessage("Zoom label created");

        modifiedLabel = new QLabel("", this);
        modifiedLabel->setMinimumWidth(80);
        statusBar()->addPermanentWidget(modifiedLabel);
        debugLogMessage("Modified label created");

        filenameLabel = new QLabel("", this);
        filenameLabel->setMinimumWidth(300);
        statusBar()->addPermanentWidget(filenameLabel);
        debugLogMessage("Filename label created");
        
        debugLogMessage("Status bar setup complete");
    } 
    catch (const std::exception& e) {
        debugLogMessage(QString("Exception in createStatusBar: %1").arg(e.what()));
    }
    catch (...) {
        debugLogMessage("Unknown exception in createStatusBar");
    }
}

// Delegate to the appropriate module managers
void MainWindow::createNewTab()
{
    fileOps->createNewTab();
}

void MainWindow::closeCurrentTab()
{
    fileOps->closeCurrentTab();
}

void MainWindow::onTabCloseRequested(int index)
{
    fileOps->onTabCloseRequested(index);
}

void MainWindow::openFile()
{
    fileOps->openFile();
}

bool MainWindow::saveFile()
{
    return fileOps->saveFile();
}

bool MainWindow::saveFileAs()
{
    return fileOps->saveFileAs();
}

bool MainWindow::maybeSaveAll()
{
    return fileOps->maybeSaveAll();
}

void MainWindow::updateTabText(int index)
{
    editorMgr->updateTabText(index);
}

void MainWindow::documentModified(bool modified)
{
    editorMgr->documentModified(modified);
}

void MainWindow::languageSelected(QAction *action)
{
    editorMgr->languageSelected(action);
}

void MainWindow::updateLanguageMenu()
{
    editorMgr->updateLanguageMenu();
}

void MainWindow::applyLightTheme()
{
    editorMgr->applyLightTheme();
}

void MainWindow::applyDarkTheme()
{
    editorMgr->applyDarkTheme();
}

void MainWindow::showFindReplaceDialog()
{
    searchMgr->showFindReplaceDialog();
}

void MainWindow::showGoToLineDialog()
{
    searchMgr->showGoToLineDialog();
}

void MainWindow::updateStatusBar()
{
    editorMgr->updateStatusBar();
}

void MainWindow::updateCursorPosition()
{
    editorMgr->updateCursorPosition();
}

void MainWindow::connectEditorSignals()
{
    editorMgr->connectEditorSignals();
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString fileName = action->data().toString();
        if (QFile::exists(fileName))
        {
            fileOps->openFileHelper(fileName);
        }
    }
}

void MainWindow::clearRecentFiles()
{
    fileOps->clearRecentFiles();
}

void MainWindow::updateRecentFilesMenu()
{
    fileOps->updateRecentFilesMenu();
}

void MainWindow::zoomIn()
{
    editorMgr->zoomIn();
}

void MainWindow::zoomOut()
{
    editorMgr->zoomOut();
}

void MainWindow::resetZoom()
{
    editorMgr->resetZoom();
}

void MainWindow::toggleWordWrap()
{
    editorMgr->toggleWordWrap();
}

void MainWindow::showAboutDialog()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("About NotepadX");
    QString htmlMessage = "<html><body>"
                          "<h2 style='margin-top:0;'><b>NotepadX</b></h2>"
                          "<p>A cross platform text editor</p>"
                          "<p>Copyright (c) 2025 by Elysian Edge LLC</p>"
                          "<p>All rights reserved.</p>"
                          "</body></html>";
    msgBox.setText(htmlMessage);
    msgBox.setTextFormat(Qt::RichText);
    msgBox.exec();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == tabWidget->tabBar())
    {
        if (event->type() == QEvent::HoverMove || event->type() == QEvent::HoverEnter)
        {
            return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
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
    
    // The modules will read their own settings
    
    // After loading settings, restore previous session
    fileOps->restoreSession();
}

void MainWindow::writeSettings()
{
    QSettings settings("NotepadX", "Editor");

    // Save window geometry
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    // Module-specific settings are handled by their respective classes
}
