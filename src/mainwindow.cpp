#include "mainwindow.h"
#include "editorwidget.h" // Make sure this include is here
#include <QShortcut>
#include <QTabBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

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
    
    // Create menu
    QMenu *fileMenu = menuBar()->addMenu("&File");
    
    // New tab action
    QAction *newTabAction = new QAction("&New", this);
    newTabAction->setShortcut(QKeySequence::New);
    fileMenu->addAction(newTabAction);
    connect(newTabAction, &QAction::triggered, this, &MainWindow::createNewTab);
    
    // Close tab action
    QAction *closeTabAction = new QAction("&Close Tab", this);
    closeTabAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_W));
    fileMenu->addAction(closeTabAction);
    connect(closeTabAction, &QAction::triggered, this, &MainWindow::closeCurrentTab);
    
    // Create initial tab
    createNewTab();
}

MainWindow::~MainWindow()
{
}

void MainWindow::createNewTab()
{
    // Create a new EditorWidget
    EditorWidget *editor = new EditorWidget(this);
    
    QString tabName = QString("Untitled %1").arg(++untitledCount);
    int index = tabWidget->addTab(editor, tabName);
    
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
    // TODO: Check for unsaved changes and prompt user
    
    QWidget *widget = tabWidget->widget(index);
    tabWidget->removeTab(index);
    delete widget;
    
    // If the last tab was closed, create a new one
    if (tabWidget->count() == 0) {
        createNewTab();
    }
}
