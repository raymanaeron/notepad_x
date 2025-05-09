#include "searchmanager.h"
#include "mainwindow.h"
#include "editorwidget.h"
#include "findreplacedialog.h"
#include "gotolinedialog.h"
#include "codeeditor.h"
#include <QTabWidget>

SearchManager::SearchManager(MainWindow *parent)
    : QObject(parent), m_mainWindow(parent), m_findReplaceDialog(nullptr), m_goToLineDialog(nullptr)
{
}

SearchManager::~SearchManager()
{
    if (m_findReplaceDialog)
    {
        delete m_findReplaceDialog;
        m_findReplaceDialog = nullptr;
    }

    if (m_goToLineDialog)
    {
        delete m_goToLineDialog;
        m_goToLineDialog = nullptr;
    }
}

void SearchManager::showFindReplaceDialog()
{
    EditorWidget *editor = currentEditor();
    if (!editor)
        return;

    if (!m_findReplaceDialog)
    {
        m_findReplaceDialog = new FindReplaceDialog(m_mainWindow);
    }

    m_findReplaceDialog->setEditor(editor->editor());
    m_findReplaceDialog->show();
    m_findReplaceDialog->raise();
    m_findReplaceDialog->activateWindow();
}

void SearchManager::showGoToLineDialog()
{
    EditorWidget *editor = currentEditor();
    if (!editor)
        return;

    if (!m_goToLineDialog)
    {
        m_goToLineDialog = new GoToLineDialog(m_mainWindow);
    }

    m_goToLineDialog->setEditor(editor->editor());
    m_goToLineDialog->show();
    m_goToLineDialog->raise();
    m_goToLineDialog->activateWindow();
}

EditorWidget *SearchManager::currentEditor()
{
    QTabWidget *tabWidget = m_mainWindow->findChild<QTabWidget*>();
    if (!tabWidget)
        return nullptr;
        
    return qobject_cast<EditorWidget *>(tabWidget->currentWidget());
}