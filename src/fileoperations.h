#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include <QObject>
#include <QStringList>
#include <QTabWidget>

class MainWindow;
class EditorWidget;

class FileOperations : public QObject
{
    Q_OBJECT

public:
    explicit FileOperations(MainWindow *parent);
    ~FileOperations();

    // File-related operations
    void createNewTab();
    void closeCurrentTab();
    void onTabCloseRequested(int index);
    void openFile();
    bool saveFile();
    bool saveFileAs();
    
    // Recent files handling
    void openRecentFile(const QString &filePath);
    void clearRecentFiles();
    void updateRecentFilesMenu();
    void addToRecentFiles(const QString &filePath);
    
    // Helpers
    bool openFileHelper(const QString &fileName);
    bool maybeSaveAll();
    bool ensureHasOpenTab();
    
    // Session management
    void saveSession();
    void restoreSession();
    
    // Getters
    QStringList getRecentFiles() const { return recentFiles; }
    
    // Create editor (moved from MainWindow)
    EditorWidget* createEditor();

private:
    MainWindow *m_mainWindow;
    QTabWidget *m_tabWidget;
    int m_untitledCount;
    QMenu *m_recentFilesMenu;
    bool m_isDarkThemeActive;
    bool m_isWordWrapEnabled;
    
    // Recent files list
    QStringList recentFiles;
    static const int MAX_RECENT_FILES = 10;
};

#endif // FILEOPERATIONS_H