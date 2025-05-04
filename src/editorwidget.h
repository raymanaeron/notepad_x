#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include <QWidget>
#include <QString>

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
    QString currentFile() const { return curFile; }
    bool isUntitled() const { return curFile.isEmpty(); }
    bool isModified() const;
    void setModified(bool modified);
    bool maybeSave();
    void setLanguage(const QString &language);
    QString currentLanguage() const;
    void setLightTheme();
    void setDarkTheme();
    
    // Add accessor method for textEditor
    CodeEditor* editor() const { return textEditor; }
    
    // Declare edit operation methods (implementation moved to cpp file)
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();

signals:
    void fileNameChanged(const QString &fileName);
    void modificationChanged(bool modified);
    void languageChanged(const QString &language);

private slots:
    void documentWasModified();

private:
    CodeEditor *textEditor;
    QString curFile;
    SyntaxHighlighter *highlighter;
    bool isDarkTheme;
    
    void setupEditor();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void updateHighlighter();
};

#endif // EDITORWIDGET_H
