#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QSettings>
#include "svgiconprovider.h"

class EditorWidget;
class QActionGroup;
class QLabel;
class QMenu;
class FileOperations;
class EditorManager;
class SearchManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    // Provide access to status bar labels for modules
    QLabel* getLineColumnLabel() { return lineColumnLabel; }
    QLabel* getZoomLabel() { return zoomLabel; }
    QLabel* getModifiedLabel() { return modifiedLabel; }
    QLabel* getFilenameLabel() { return filenameLabel; }

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

public slots:
    // These slots need to be public so they can be called from module classes
    void updateTabText(int index);
    void documentModified(bool modified);
    void updateLanguageMenu();
    void connectEditorSignals();
    void updateStatusBar();
    void updateCursorPosition();

private slots:
    void createNewTab();
    void closeCurrentTab();
    void onTabCloseRequested(int index);
    void openFile();
    bool saveFile();
    bool saveFileAs();
    void languageSelected(QAction *action);
    void applyLightTheme();
    void applyDarkTheme();
    
    // Find and Go to Line slots
    void showFindReplaceDialog();
    void showGoToLineDialog();
    
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
    
    // Add word wrap slot
    void toggleWordWrap();

private:
    // Core UI components
    QTabWidget *tabWidget;
    
    // Status bar labels
    QLabel *lineColumnLabel;
    QLabel *zoomLabel;
    QLabel *modifiedLabel;
    QLabel *filenameLabel;
      // Module classes that handle specific functionality
    FileOperations *fileOps;
    EditorManager *editorMgr;
    SearchManager *searchMgr;
      // UI components
    QActionGroup *languageActionGroup;
    QActionGroup *themeActionGroup;
    
    // UI setup methods
    void createMenus();
    void createToolBar();
    void createTabWidget();
    void createStatusBar();
    
    // Settings handling
    void readSettings();
    void writeSettings();
    
    // Event handling
    bool maybeSaveAll();
};

#endif // MAINWINDOW_H
