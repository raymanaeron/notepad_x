#include "mainwindow.h"
#include "editorwidget.h" 
#include "highlighting/highlighterfactory.h"
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), untitledCount(0), isDarkThemeActive(false)
{
    setWindowTitle("Notepad X");
    resize(800, 600);
    
    // Create tab widget
    tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    setCentralWidget(tabWidget);
    
    // Connect the tabCloseRequested signal
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);
    connect(tabWidget, &QTabWidget::currentChanged, this, &MainWindow::updateTabText);
    
    // Create status bar
    statusBar();
    
    // Create menus and actions
    createMenus();
    
    // Create initial tab
    createNewTab();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSaveAll()) {
        event->accept();
    } else {
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
    connect(exitAction, &QAction::triggered, this, [this]() {
        if (maybeSaveAll()) {
            qApp->quit();
        }
    });
    
    // Create Language menu
    languageMenu = menuBar()->addMenu("&Language");
    languageActionGroup = new QActionGroup(this);
    connect(languageActionGroup, &QActionGroup::triggered, this, &MainWindow::languageSelected);
    
    // Populate language menu
    QStringList languages = HighlighterFactory::instance().supportedLanguages();
    
    for (const QString &lang : languages) {
        QAction *langAction = languageMenu->addAction(lang);
        langAction->setCheckable(true);
        
        // Default to "None" selected
        if (lang == "None") {
            langAction->setChecked(true);
        }
        
        languageActionGroup->addAction(langAction);
    }
    
    // Connect tab change signal to update language menu
    connect(tabWidget, &QTabWidget::currentChanged, this, &MainWindow::updateLanguageMenu);
    
    // Create View menu with Theme submenu
    QMenu *viewMenu = menuBar()->addMenu("&View");
    QMenu *themeMenu = viewMenu->addMenu("&Theme");
    
    // Create theme action group
    themeActionGroup = new QActionGroup(this);
    
    // Light theme action
    QAction *lightThemeAction = new QAction("&Light Theme", this);
    lightThemeAction->setCheckable(true);
    lightThemeAction->setChecked(true); // Default is light theme
    themeActionGroup->addAction(lightThemeAction);
    themeMenu->addAction(lightThemeAction);
    connect(lightThemeAction, &QAction::triggered, this, &MainWindow::applyLightTheme);
    
    // Dark theme action
    QAction *darkThemeAction = new QAction("&Dark Theme", this);
    darkThemeAction->setCheckable(true);
    themeActionGroup->addAction(darkThemeAction);
    themeMenu->addAction(darkThemeAction);
    connect(darkThemeAction, &QAction::triggered, this, &MainWindow::applyDarkTheme);
}

void MainWindow::createNewTab()
{
    // Create a new EditorWidget
    EditorWidget *editor = new EditorWidget(this);
    
    QString tabName = QString("Untitled %1").arg(++untitledCount);
    int index = tabWidget->addTab(editor, tabName);
    
    // Connect signals
    connect(editor, &EditorWidget::fileNameChanged, this, [=](const QString &fileName) {
        QString tabText = fileName.isEmpty() ? 
                        QString("Untitled %1").arg(untitledCount) : 
                        QFileInfo(fileName).fileName();
        tabWidget->setTabText(tabWidget->indexOf(editor), tabText);
    });
    
    connect(editor, &EditorWidget::modificationChanged, this, &MainWindow::documentModified);
    
    connect(editor, &EditorWidget::languageChanged, this, [=](const QString &) {
        // Update language menu when the language changes
        if (editor == currentEditor()) {
            updateLanguageMenu();
        }
    });
    
    // Set the new tab as the current tab
    tabWidget->setCurrentIndex(index);
    editor->setFocus();
    
    // Apply the active theme to the new tab
    if (isDarkThemeActive) {
        editor->setDarkTheme();
    } else {
        editor->setLightTheme();
    }
    
    // Update the language menu to reflect the current editor
    updateLanguageMenu();
}

void MainWindow::closeCurrentTab()
{
    int currentIndex = tabWidget->currentIndex();
    if (currentIndex != -1) {
        onTabCloseRequested(currentIndex);
    }
}

void MainWindow::onTabCloseRequested(int index)
{
    EditorWidget *editor = qobject_cast<EditorWidget*>(tabWidget->widget(index));
    if (!editor)
        return;
        
    if (editor->maybeSave()) {
        tabWidget->removeTab(index);
        delete editor;
        
        // If the last tab was closed, create a new one
        if (tabWidget->count() == 0) {
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
        
    // Check if the file is already open
    for (int i = 0; i < tabWidget->count(); ++i) {
        EditorWidget *editor = qobject_cast<EditorWidget*>(tabWidget->widget(i));
        if (editor && QFileInfo(fileName) == QFileInfo(editor->currentFile())) {
            tabWidget->setCurrentIndex(i);
            return;
        }
    }
    
    // If current tab is untitled and not modified, use it
    EditorWidget *currentEditor = qobject_cast<EditorWidget*>(tabWidget->currentWidget());
    if (currentEditor && currentEditor->isUntitled() && !currentEditor->isModified()) {
        if (currentEditor->loadFile(fileName)) {
            statusBar()->showMessage(tr("File loaded"), 2000);
        }
        return;
    }
    
    // Create a new tab
    EditorWidget *editor = new EditorWidget(this);
    if (editor->loadFile(fileName)) {
        int index = tabWidget->addTab(editor, QFileInfo(fileName).fileName());
        tabWidget->setCurrentIndex(index);
        
        // Connect signals
        connect(editor, &EditorWidget::fileNameChanged, this, [=](const QString &fileName) {
            QString tabText = fileName.isEmpty() ? 
                            QString("Untitled %1").arg(untitledCount) : 
                            QFileInfo(fileName).fileName();
            tabWidget->setTabText(tabWidget->indexOf(editor), tabText);
        });
        
        connect(editor, &EditorWidget::modificationChanged, this, &MainWindow::documentModified);
        statusBar()->showMessage(tr("File loaded"), 2000);
        
        // Apply the active theme to this newly opened file
        if (isDarkThemeActive) {
            editor->setDarkTheme();
        } else {
            editor->setLightTheme();
        }
    } else {
        delete editor;
    }
}

bool MainWindow::saveFile()
{
    if (!ensureHasOpenTab())
        return false;
        
    EditorWidget *editor = currentEditor();
    if (!editor)
        return false;
        
    if (editor->save()) {
        statusBar()->showMessage(tr("File saved"), 2000);
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
        
    if (editor->saveAs()) {
        statusBar()->showMessage(tr("File saved"), 2000);
        return true;
    }
    
    return false;
}

EditorWidget* MainWindow::currentEditor()
{
    return qobject_cast<EditorWidget*>(tabWidget->currentWidget());
}

bool MainWindow::ensureHasOpenTab()
{
    if (tabWidget->count() == 0) {
        createNewTab();
    }
    return tabWidget->count() > 0;
}

void MainWindow::updateTabText(int index)
{
    if (index < 0)
        return;
        
    EditorWidget *editor = qobject_cast<EditorWidget*>(tabWidget->widget(index));
    if (!editor)
        return;
        
    // Update window title to show current file
    if (index == tabWidget->currentIndex()) {
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
    EditorWidget *editor = qobject_cast<EditorWidget*>(sender());
    if (!editor)
        return;
        
    int index = tabWidget->indexOf(editor);
    if (index < 0)
        return;
        
    QString tabText = tabWidget->tabText(index);
    if (modified && !tabText.endsWith(" *")) {
        tabWidget->setTabText(index, tabText + " *");
    } else if (!modified && tabText.endsWith(" *")) {
        tabWidget->setTabText(index, tabText.left(tabText.length() - 2));
    }
    
    // Also update window title if this is the current tab
    if (index == tabWidget->currentIndex()) {
        updateTabText(index);
    }
}

bool MainWindow::maybeSaveAll()
{
    for (int i = 0; i < tabWidget->count(); ++i) {
        EditorWidget *editor = qobject_cast<EditorWidget*>(tabWidget->widget(i));
        if (editor && editor->isModified()) {
            tabWidget->setCurrentIndex(i);
            if (!editor->maybeSave()) {
                return false;
            }
        }
    }
    return true;
}

void MainWindow::languageSelected(QAction *action)
{
    EditorWidget *editor = currentEditor();
    if (editor) {
        editor->setLanguage(action->text());
    }
}

void MainWindow::updateLanguageMenu()
{
    EditorWidget *editor = currentEditor();
    if (editor) {
        QString currentLang = editor->currentLanguage();
        
        // Find and check the appropriate action
        for (QAction *action : languageActionGroup->actions()) {
            if (action->text() == currentLang) {
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
    
    // Create and apply light theme palette
    QPalette lightPalette;
    qApp->setPalette(lightPalette);
    
    // Update theme tracking state
    isDarkThemeActive = false;
    
    // Update all existing editors
    for (int i = 0; i < tabWidget->count(); ++i) {
        EditorWidget *editor = qobject_cast<EditorWidget*>(tabWidget->widget(i));
        if (editor) {
            // Update editor theme
            editor->setLightTheme();
        }
    }
}

void MainWindow::applyDarkTheme()
{
    // Set dark theme for the application
    qApp->setStyle("Fusion");
    
    // Create and apply dark theme palette
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(35, 35, 35));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    
    qApp->setPalette(darkPalette);
    
    // Update theme tracking state
    isDarkThemeActive = true;
    
    // Update all existing editors
    for (int i = 0; i < tabWidget->count(); ++i) {
        EditorWidget *editor = qobject_cast<EditorWidget*>(tabWidget->widget(i));
        if (editor) {
            // Update editor theme
            editor->setDarkTheme();
        }
    }
}
