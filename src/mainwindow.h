#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>

class EditorWidget;
class QActionGroup;
class FindReplaceDialog;
class GoToLineDialog;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void createNewTab();
    void closeCurrentTab();
    void onTabCloseRequested(int index);
    void openFile();
    bool saveFile();
    bool saveFileAs();
    void updateTabText(int index);
    void documentModified(bool modified);
    void languageSelected(QAction *action);
    void updateLanguageMenu();
    void applyLightTheme();
    void applyDarkTheme();
    
    // Find and Go to Line slots
    void showFindReplaceDialog();
    void showGoToLineDialog();
    
    // Status bar update slots
    void updateStatusBar();
    void updateCursorPosition();

private:
    QTabWidget *tabWidget;
    int untitledCount;
    QMenu *languageMenu;
    QActionGroup *languageActionGroup;
    QActionGroup *themeActionGroup;
    bool isDarkThemeActive;  // Track the currently active theme
    
    // Status bar labels
    QLabel *lineColumnLabel;  // Shows line and column position
    QLabel *modifiedLabel;    // Shows modified status
    QLabel *filenameLabel;    // Shows filename or path
    
    // Dialogs
    FindReplaceDialog *findReplaceDialog;
    GoToLineDialog *goToLineDialog;
    
    void createActions();
    void createMenus();
    void createToolBar();
    void createStatusBar();
    void connectEditorSignals();
    EditorWidget *currentEditor();
    bool ensureHasOpenTab();
    bool maybeSaveAll();
};

#endif // MAINWINDOW_H
