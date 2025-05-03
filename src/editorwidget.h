#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include <QWidget>
#include "codeeditor.h"

class EditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditorWidget(QWidget *parent = nullptr);
    
    CodeEditor* editor() const { return textEditor; }
    
    // File handling methods
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    bool maybeSave();
    
    QString currentFile() const { return curFile; }
    bool isUntitled() const { return curFile.isEmpty(); }
    bool isModified() const;
    void setModified(bool modified);

signals:
    void fileNameChanged(const QString &fileName);
    void modificationChanged(bool modified);

private slots:
    void documentWasModified();

private:
    CodeEditor *textEditor;
    QString curFile;
    
    void setupEditor();
    void setCurrentFile(const QString &fileName);
};

#endif // EDITORWIDGET_H
