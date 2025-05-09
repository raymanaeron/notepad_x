#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H

#include <QObject>

class MainWindow;
class EditorWidget;
class FindReplaceDialog;
class GoToLineDialog;

class SearchManager : public QObject
{
    Q_OBJECT

public:
    explicit SearchManager(MainWindow *parent);
    ~SearchManager();

    // Dialog handling
    void showFindReplaceDialog();
    void showGoToLineDialog();

private:
    MainWindow *m_mainWindow;
    FindReplaceDialog *m_findReplaceDialog;
    GoToLineDialog *m_goToLineDialog;

    // Helper to get current editor
    EditorWidget *currentEditor();
};

#endif // SEARCHMANAGER_H