#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include <QWidget>
#include <QString>
#include <QVBoxLayout>  // Add this include for QVBoxLayout
#include <QTextOption>  // Add this include for QTextOption

class CodeEditor;
class SyntaxHighlighter;

class EditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditorWidget(QWidget *parent = nullptr);
    
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    QString currentFile() const { return currentFilePath; }  // Changed from curFile to currentFilePath
    bool isUntitled() const { return currentFilePath.isEmpty(); }  // Changed from curFile to currentFilePath
    bool isModified() const;
    void setModified(bool modified);
    bool maybeSave();
    void setLanguage(const QString &language);
    QString currentLanguage() const;
    void setLightTheme();
    void setDarkTheme();
    bool isDarkTheme() const { return usingDarkTheme; }
    
    // Add accessor method for textEditor
    CodeEditor* editor() const { return textEditor; }
    
    // Declare edit operation methods
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();

    // Add zoom methods
    void zoomIn(int range = 1);
    void zoomOut(int range = 1);
    void resetZoom();
    int getCurrentZoomLevel() const;
    void setZoomLevel(int level);

    // Add methods for word wrap support
    void setWordWrapMode(QTextOption::WrapMode mode);
    QTextOption::WrapMode wordWrapMode() const;

signals:
    void fileNameChanged(const QString &fileName);
    void modificationChanged(bool modified);
    void languageChanged(const QString &language);
    void zoomLevelChanged(int level);

private slots:
    void documentWasModified();

private:
    CodeEditor *textEditor;
    QVBoxLayout *layout;
    QString currentFilePath;
    QString currentLang;
    SyntaxHighlighter *highlighter;
    bool usingDarkTheme;
    
    void setupEditor();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void updateHighlighter();
    void initEditor(); // Add this declaration for the initEditor() method
};

#endif // EDITORWIDGET_H
