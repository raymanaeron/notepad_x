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

private:
    CodeEditor *textEditor;
    
    void setupEditor();
};

#endif // EDITORWIDGET_H
