#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTabWidget>

class MainWindow;
class EditorWidget;
class QMenu;

class FileOperations : public QObject
{
    Q_OBJECT

public:
    explicit FileOperations(MainWindow *parent);
    ~FileOperations();

    // Tab operations
    void createNewTab();
    void closeCurrentTab();
    void onTabCloseRequested(int index);

    // File operations
    void openFile();
    bool openFileHelper(const QString &fileName);
    bool saveFile();
    bool saveFileAs();
    bool maybeSaveAll();

    // Session management
    void saveSession();
    void restoreSession();
    
    // Word wrap setting
    void setWordWrapEnabled(bool enabled);

public slots:
    // Recent files operations
    void addToRecentFiles(const QString &filePath);
    void openRecentFile(const QString &filePath);
    void clearRecentFiles();
    void updateRecentFilesMenu();

    // Getters
    QStringList getRecentFiles() const { return recentFiles; }    // Create editor (moved from MainWindow)
    EditorWidget* createEditor();
    
    // Ensure at least one tab is open
    bool ensureHasOpenTab();

private:
    MainWindow *m_mainWindow;
    QTabWidget *m_tabWidget;
    QMenu *m_recentFilesMenu;
    QStringList recentFiles;
    int m_untitledCount;

    // Keep track of theme and word wrap settings
    bool m_isDarkThemeActive;
    bool m_isWordWrapEnabled;

    static const int MAX_RECENT_FILES = 10;
};

#endif // FILEOPERATIONS_H