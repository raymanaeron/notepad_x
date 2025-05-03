#include "mainwindow.h"
#include "editorwidget.h" 
#include <QShortcut>
#include <QTabBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QStatusBar> // Add this include for QStatusBar

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), untitledCount(0)
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
    // Create menu
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
    
    // Set the new tab as the current tab
    tabWidget->setCurrentIndex(index);
    editor->setFocus();
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
    
    // Otherwise, create a new tab
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
