#include "mainwindow.h"
#include "editorwidget.h"
#include "highlighting/highlighterfactory.h"
#include "findreplacedialog.h"
#include "gotolinedialog.h"
#include "codeeditor.h"
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
#include "fonticon.h"
#include "svgiconprovider.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), untitledCount(0), isDarkThemeActive(false),
      findReplaceDialog(nullptr), goToLineDialog(nullptr), currentZoomLevel(0),
      isWordWrapEnabled(false)
{
    setWindowTitle("NotepadX");

    createTabWidget();
    createMenus();
    createToolBar();
    createStatusBar();
    readSettings();  // This loads recent files and other settings
    
    // Only create a new tab if no tabs were restored from session
    if (tabWidget->count() == 0)
    {
        createNewTab();
    }

    if (isDarkThemeActive)
    {
        applyDarkTheme();
    }
}

MainWindow::~MainWindow()
{
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
        saveSession();  // Save session before closing
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

    recentFilesMenu = fileMenu->addMenu("Recent &Files");

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
        EditorWidget *editor = currentEditor();
        if (editor) editor->undo(); });

    QAction *redoAction = new QAction("&Redo", this);
    redoAction->setShortcut(QKeySequence::Redo);
    editMenu->addAction(redoAction);
    connect(redoAction, &QAction::triggered, this, [this]()
            {
        EditorWidget *editor = currentEditor();
        if (editor) editor->redo(); });

    editMenu->addSeparator();

    QAction *cutAction = new QAction("Cu&t", this);
    cutAction->setShortcut(QKeySequence::Cut);
    editMenu->addAction(cutAction);
    connect(cutAction, &QAction::triggered, this, [this]()
            {
        EditorWidget *editor = currentEditor();
        if (editor) editor->cut(); });

    QAction *copyAction = new QAction("&Copy", this);
    copyAction->setShortcut(QKeySequence::Copy);
    editMenu->addAction(copyAction);
    connect(copyAction, &QAction::triggered, this, [this]()
            {
        EditorWidget *editor = currentEditor();
        if (editor) editor->copy(); });

    QAction *pasteAction = new QAction("&Paste", this);
    pasteAction->setShortcut(QKeySequence::Paste);
    editMenu->addAction(pasteAction);
    connect(pasteAction, &QAction::triggered, this, [this]()
            {
        EditorWidget *editor = currentEditor();
        if (editor) editor->paste(); });

    editMenu->addSeparator();

    QAction *selectAllAction = new QAction("Select &All", this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    editMenu->addAction(selectAllAction);
    connect(selectAllAction, &QAction::triggered, this, [this]()
            {
        EditorWidget *editor = currentEditor();
        if (editor) editor->selectAll(); });

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

    themeActionGroup = new QActionGroup(this);

    QAction *lightThemeAction = new QAction("&Light Theme", this);
    lightThemeAction->setCheckable(true);
    lightThemeAction->setChecked(!isDarkThemeActive);
    themeActionGroup->addAction(lightThemeAction);
    themeMenu->addAction(lightThemeAction);
    connect(lightThemeAction, &QAction::triggered, this, &MainWindow::applyLightTheme);

    QAction *darkThemeAction = new QAction("&Dark Theme", this);
    darkThemeAction->setCheckable(true);
    darkThemeAction->setChecked(isDarkThemeActive);
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

    wordWrapAction = new QAction("&Word Wrap", this);
    wordWrapAction->setCheckable(true);
    wordWrapAction->setChecked(isWordWrapEnabled);
    viewMenu->addAction(wordWrapAction);
    connect(wordWrapAction, &QAction::triggered, this, &MainWindow::toggleWordWrap);

    languageMenu = menuBar()->addMenu("&Language");
    languageActionGroup = new QActionGroup(this);
    connect(languageActionGroup, &QActionGroup::triggered, this, &MainWindow::languageSelected);

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
        EditorWidget *editor = currentEditor();
        if (editor) editor->cut(); });

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

    if (editor->isModified())
    {
        modifiedLabel->setText("[Modified]");
    }
    else
    {
        modifiedLabel->setText("");
    }

    QString filename = editor->currentFile();
    if (filename.isEmpty())
    {
        filenameLabel->setText(QString("Untitled %1").arg(untitledCount));
    }
    else
    {
        filenameLabel->setText(filename);
    }

    int zoomLevel = editor->getCurrentZoomLevel();
    int zoomPercent = 100 + (zoomLevel * 10);
    zoomLabel->setText(QString("Zoom: %1%").arg(zoomPercent));

    CodeEditor *codeEditor = editor->editor();
    if (codeEditor)
    {
        QTextCursor cursor = codeEditor->textCursor();
        int line = cursor.blockNumber() + 1;
        int column = cursor.columnNumber() + 1;
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
        int line = cursor.blockNumber() + 1;
        int column = cursor.columnNumber() + 1;
        lineColumnLabel->setText(QString("Line: %1, Column: %2").arg(line).arg(column));
    }
}

void MainWindow::createNewTab()
{
    EditorWidget *editor = createEditor();

    QString tabName = QString("Untitled %1").arg(++untitledCount);
    int index = tabWidget->addTab(editor, tabName);

    connect(editor, &EditorWidget::fileNameChanged, this, [=](const QString &fileName)
            {
        QString tabText = fileName.isEmpty() ? 
                        QString("Untitled %1").arg(untitledCount) : 
                        QFileInfo(fileName).fileName();
        tabWidget->setTabText(tabWidget->indexOf(editor), tabText);
        if (editor == currentEditor()) {
            updateStatusBar();
        } });

    connect(editor, &EditorWidget::modificationChanged, this, &MainWindow::documentModified);

    connect(editor, &EditorWidget::languageChanged, this, [=](const QString &)
            {
        if (editor == currentEditor()) {
            updateLanguageMenu();
        } });

    connect(editor, &EditorWidget::zoomLevelChanged, this, [=](int)
            {
        if (editor == currentEditor()) {
            updateStatusBar();
        } });

    tabWidget->setCurrentIndex(index);
    editor->setFocus();

    if (isDarkThemeActive)
    {
        editor->setDarkTheme();
    }
    else
    {
        editor->setLightTheme();
    }

    updateLanguageMenu();
    connectEditorSignals();
}

EditorWidget* MainWindow::createEditor()
{
    EditorWidget *editor = new EditorWidget(this);

    // Apply current word wrap setting
    editor->setWordWrapMode(isWordWrapEnabled ? QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::NoWrap);

    return editor;
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
        addToRecentFiles(fileName);
    }
}

bool MainWindow::openFileHelper(const QString &fileName)
{
    if (fileName.isEmpty())
        return false;

    for (int i = 0; i < tabWidget->count(); ++i)
    {
        EditorWidget *editor = qobject_cast<EditorWidget *>(tabWidget->widget(i));
        if (editor && QFileInfo(fileName) == QFileInfo(editor->currentFile()))
        {
            tabWidget->setCurrentIndex(i);
            return true;
        }
    }

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

    EditorWidget *editor = createEditor();
    if (editor->loadFile(fileName))
    {
        int index = tabWidget->addTab(editor, QFileInfo(fileName).fileName());
        tabWidget->setCurrentIndex(index);

        connect(editor, &EditorWidget::fileNameChanged, this, [=](const QString &fileName)
                {
            QString tabText = fileName.isEmpty() ? 
                            QString("Untitled %1").arg(untitledCount) : 
                            QFileInfo(fileName).fileName();
            tabWidget->setTabText(tabWidget->indexOf(editor), tabText);
            if (editor == currentEditor()) {
                updateStatusBar();
            } });

        connect(editor, &EditorWidget::modificationChanged, this, &MainWindow::documentModified);

        connect(editor, &EditorWidget::zoomLevelChanged, this, [=](int)
                {
            if (editor == currentEditor()) {
                updateStatusBar();
            } });

        statusBar()->showMessage(tr("File loaded"), 2000);

        if (isDarkThemeActive)
        {
            editor->setDarkTheme();
        }
        else
        {
            editor->setLightTheme();
        }

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

    if (index == tabWidget->currentIndex())
    {
        QString title = editor->currentFile();
        if (title.isEmpty())
            title = QString("Untitled %1").arg(untitledCount);
        else
            title = QFileInfo(title).fileName();

        if (editor->isModified())
            title += " *";

        title += " - NotepadX";
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

    if (index == tabWidget->currentIndex())
    {
        updateTabText(index);
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
    qApp->setStyle("Fusion");
    setStyleSheet("");

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

    isDarkThemeActive = false;

    for (int i = 0; i < tabWidget->count(); ++i)
    {
        EditorWidget *editor = qobject_cast<EditorWidget *>(tabWidget->widget(i));
        if (editor)
        {
            editor->setLightTheme();
        }
    }
    
    // Set toolbar icons to appropriate color for light theme
    QList<QAction *> actions = findChildren<QAction *>();
    for (QAction *action : actions)
    {
        if (action->icon().isNull())
            continue;

        QIcon originalIcon = action->icon();
        QIcon::Mode mode = QIcon::Normal;

        QList<QSize> sizes = originalIcon.availableSizes(mode);
        if (sizes.isEmpty())
            sizes.append(QSize(24, 24));

        QIcon newIcon;
        for (const QSize &size : sizes)
        {
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
    settings.setValue("darkTheme", isDarkThemeActive);
}

void MainWindow::applyDarkTheme()
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
    QList<QAction *> actions = findChildren<QAction *>();
    for (QAction *action : actions)
    {
        if (action->icon().isNull())
            continue;

        QIcon originalIcon = action->icon();
        QIcon::Mode mode = QIcon::Normal;

        QList<QSize> sizes = originalIcon.availableSizes(mode);
        if (sizes.isEmpty())
            sizes.append(QSize(24, 24));

        QIcon newIcon;
        for (const QSize &size : sizes)
        {
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
        "QMenuBar { background-color: rgb(51, 51, 51); color: rgb(220, 220, 220); } "
        "QMenuBar::item:selected { background-color: rgb(60, 60, 60); } "
        "QMenu { background-color: rgb(51, 51, 51); color: rgb(220, 220, 220); border: 1px solid rgb(64, 64, 64); } "
        "QMenu::item:selected { background-color: rgb(60, 60, 60); } "
        "QToolBar { background-color: rgb(51, 51, 51); border: none; } "
        "QToolBar::separator { background-color: rgb(80, 80, 80); width: 1px; margin: 4px 4px; } "
        "QStatusBar { background-color: rgb(51, 51, 51); color: rgb(220, 220, 220); }"
        "QTabBar::tab { background-color: rgb(51, 51, 51); color: rgb(220, 220, 220); border: 1px solid rgb(64, 64, 64); padding: 5px; }"
        "QTabBar::tab:selected { background-color: rgb(30, 30, 30); }"
    );

    setStyleSheet(styleSheet);

    isDarkThemeActive = true;

    for (int i = 0; i < tabWidget->count(); ++i)
    {
        EditorWidget *editor = qobject_cast<EditorWidget *>(tabWidget->widget(i));
        if (editor)
        {
            editor->setDarkTheme();
        }
    }

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

    if (!findReplaceDialog)
    {
        findReplaceDialog = new FindReplaceDialog(this);
    }

    findReplaceDialog->setEditor(editor->editor());
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

    if (!goToLineDialog)
    {
        goToLineDialog = new GoToLineDialog(this);
    }

    goToLineDialog->setEditor(editor->editor());
    goToLineDialog->show();
    goToLineDialog->raise();
    goToLineDialog->activateWindow();
}

void MainWindow::connectEditorSignals()
{
    EditorWidget *editor = currentEditor();
    if (!editor)
        return;

    CodeEditor *codeEditor = editor->editor();
    if (codeEditor)
    {
        disconnect(codeEditor, &CodeEditor::cursorPositionChanged, this, nullptr);
        connect(codeEditor, &CodeEditor::cursorPositionChanged, this, &MainWindow::updateCursorPosition);
        
        disconnect(codeEditor, &CodeEditor::zoomLevelChanged, this, nullptr);
        connect(codeEditor, &CodeEditor::zoomLevelChanged, this, [this](int)
                { updateStatusBar(); });
    }

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
    
    // Restore word wrap setting
    isWordWrapEnabled = settings.value("wordWrap", false).toBool();

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

    // Update word wrap menu item to match the loaded preference
    QList<QAction *> actions = findChildren<QAction *>();
    for (QAction *action : actions)
    {
        if (action->text() == "&Word Wrap")
        {
            action->setChecked(isWordWrapEnabled);
            break;
        }
    }

    // Restore recent files list with validation
    QStringList validRecentFiles;
    recentFiles = settings.value("recentFiles").toStringList();
    for (const QString &filePath : recentFiles)
    {
        if (QFile::exists(filePath))
        {
            validRecentFiles.append(filePath);
        }
    }
    recentFiles = validRecentFiles;
    
    updateRecentFilesMenu();
    
    // After loading settings, restore previous session
    restoreSession();
}

void MainWindow::writeSettings()
{
    QSettings settings("NotepadX", "Editor");

    // Save window geometry
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    // Save theme setting
    settings.setValue("darkTheme", isDarkThemeActive);

    // Save word wrap setting
    settings.setValue("wordWrap", isWordWrapEnabled);

    // Save recent files
    settings.setValue("recentFiles", recentFiles);
}

void MainWindow::addToRecentFiles(const QString &filePath)
{
    recentFiles.removeAll(filePath);
    recentFiles.prepend(filePath);
    
    // Limit to MAX_RECENT_FILES
    while (recentFiles.size() > MAX_RECENT_FILES) {
        recentFiles.removeLast();
    }
    
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
            openFileHelper(fileName);
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
        return;

    recentFilesMenu->clear();
    
    // First add the "Clear Recent Files" action
    QAction *clearRecentAction = new QAction("&Clear Recent Files", this);
    connect(clearRecentAction, &QAction::triggered, this, &MainWindow::clearRecentFiles);
    recentFilesMenu->addAction(clearRecentAction);
    
    if (!recentFiles.isEmpty()) {
        recentFilesMenu->addSeparator();
        
        for (const QString &file : recentFiles)
        {
            QAction *action = new QAction(QFileInfo(file).fileName(), this);
            action->setData(file);
            action->setToolTip(file); // Show full path on hover
            connect(action, &QAction::triggered, this, &MainWindow::openRecentFile);
            recentFilesMenu->addAction(action);
        }
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

void MainWindow::toggleWordWrap()
{
    isWordWrapEnabled = !isWordWrapEnabled;
    wordWrapAction->setChecked(isWordWrapEnabled);  // Update menu item state
    
    QTextOption::WrapMode mode = isWordWrapEnabled ? 
        QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::NoWrap;
    
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        EditorWidget *editor = qobject_cast<EditorWidget *>(tabWidget->widget(i));
        if (editor)
        {
            editor->setWordWrapMode(mode);
        }
    }
    
    // Save the setting
    QSettings settings("NotepadX", "Editor");
    settings.setValue("wordWrap", isWordWrapEnabled);
}

void MainWindow::saveSession()
{
    QSettings settings("NotepadX", "Editor");
    QStringList openFiles;
    int currentIndex = tabWidget->currentIndex();
    
    settings.beginWriteArray("openFiles");
    for (int i = 0; i < tabWidget->count(); ++i) {
        EditorWidget *editor = qobject_cast<EditorWidget *>(tabWidget->widget(i));
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

void MainWindow::restoreSession()
{
    QSettings settings("NotepadX", "Editor");
    int size = settings.beginReadArray("openFiles");
    
    // Don't create default tab if we're going to restore files
    if (size > 0) {
        // Remove the default tab if it exists and is empty/untitled
        if (tabWidget->count() == 1) {
            EditorWidget *editor = qobject_cast<EditorWidget *>(tabWidget->widget(0));
            if (editor && editor->isUntitled() && !editor->isModified()) {
                tabWidget->removeTab(0);
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
                tabTitle = QString("Untitled %1").arg(++untitledCount);
            } else {
                tabTitle = QFileInfo(filePath).fileName();
            }
            
            int index = tabWidget->addTab(editor, tabTitle);
            
            // Set up the necessary connections
            connect(editor, &EditorWidget::fileNameChanged, this, [=](const QString &fileName) {
                QString tabText = fileName.isEmpty() ? 
                                QString("Untitled %1").arg(untitledCount) : 
                                QFileInfo(fileName).fileName();
                tabWidget->setTabText(tabWidget->indexOf(editor), tabText);
                if (editor == currentEditor()) {
                    updateStatusBar();
                }
            });
            
            connect(editor, &EditorWidget::modificationChanged, this, &MainWindow::documentModified);
            connect(editor, &EditorWidget::languageChanged, this, [=](const QString &) {
                if (editor == currentEditor()) {
                    updateLanguageMenu();
                }
            });
            connect(editor, &EditorWidget::zoomLevelChanged, this, [=](int) {
                if (editor == currentEditor()) {
                    updateStatusBar();
                }
            });
            
            if (isDarkThemeActive) {
                editor->setDarkTheme();
            }
        }
        
        // Select previously active tab
        int activeTab = settings.value("activeTab", 0).toInt();
        if (activeTab >= 0 && activeTab < tabWidget->count()) {
            tabWidget->setCurrentIndex(activeTab);
        }
    }
    settings.endArray();
    
    connectEditorSignals();
    updateLanguageMenu();
    updateStatusBar();
}
