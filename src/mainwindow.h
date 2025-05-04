#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QSettings>
#include "svgiconprovider.h"

class EditorWidget;
class QActionGroup;
class FindReplaceDialog;
class GoToLineDialog;
class QLabel;
class QMenu;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override; // Add this line

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
    
    // Recent files related slots
    void openRecentFile();
    void clearRecentFiles();
    void updateRecentFilesMenu();
    
    // Add zoom slots
    void zoomIn();
    void zoomOut();
    void resetZoom();
    
    // About dialog slot
    void showAboutDialog();

private:
    QTabWidget *tabWidget;
    int untitledCount;
    QMenu *languageMenu;
    QMenu *recentFilesMenu;
    QActionGroup *languageActionGroup;
    QActionGroup *themeActionGroup;
    bool isDarkThemeActive;  // Track the currently active theme
    
    // Recent files list
    QStringList recentFiles;
    const int maxRecentFiles = 10;
    
    // Status bar labels
    QLabel *lineColumnLabel;  // Shows line and column position
    QLabel *modifiedLabel;    // Shows modified status
    QLabel *filenameLabel;    // Shows filename or path
    QLabel *zoomLabel;        // Shows current zoom level
    
    // Dialogs
    FindReplaceDialog *findReplaceDialog;
    GoToLineDialog *goToLineDialog;
    
    int currentZoomLevel;
    
    void createMenus();
    void createToolBar();
    void createStatusBar();
    void connectEditorSignals();
    EditorWidget *currentEditor();
    bool ensureHasOpenTab();
    bool maybeSaveAll();
    
    // Settings handling
    void readSettings();
    void writeSettings();
    void addToRecentFiles(const QString &filePath);
    bool openFileHelper(const QString &fileName);
};

#endif // MAINWINDOW_H
