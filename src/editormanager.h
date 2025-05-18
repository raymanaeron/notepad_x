#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include <QObject>
#include <QList>
#include <QString>

class MainWindow;
class EditorWidget;
class CodeEditor;
class QTabWidget;
class QLabel;
class QAction;
class QActionGroup;
class QMenu;

class EditorManager : public QObject
{
    Q_OBJECT
    
public:
    explicit EditorManager(MainWindow *parent);
    ~EditorManager();
    
    EditorWidget* currentEditor();
    bool isDarkTheme() const { return m_isDarkThemeActive; }
    
public slots:
    // Tab and document management
    void updateTabText(int index);
    void documentModified(bool modified);
    void updateLanguageMenu();
    void connectEditorSignals();
    
    // Language and theme handling
    void languageSelected(QAction *action);
    void applyLightTheme();
    void applyDarkTheme();
    
    // Status bar updates
    void updateStatusBar();
    void updateCursorPosition();
      // View options
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void toggleWordWrap();
    void updateWordWrapState(); // New method to ensure word wrap state is applied
    
private:
    MainWindow *m_mainWindow;
    QTabWidget *m_tabWidget;
    
    // Menu and action references
    QMenu *m_languageMenu;
    QActionGroup *m_languageActionGroup;
    QActionGroup *m_themeActionGroup;
    QAction *m_wordWrapAction;
    
    // Status bar label references - these will be set directly from MainWindow
    QLabel *m_lineColumnLabel;
    QLabel *m_modifiedLabel;
    QLabel *m_filenameLabel;
    QLabel *m_zoomLabel;
    
    // Settings
    int m_currentZoomLevel;
    bool m_isDarkThemeActive;
    bool m_isWordWrapEnabled;
};

#endif // EDITORMANAGER_H