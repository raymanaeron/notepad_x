#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

class LineNumberArea;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);
    
    // Expose the protected setViewportMargins method
    void setEditorMargins(int left, int top, int right, int bottom);
    
    // Method for line number area to use during painting
    int lineNumberAreaWidth();
    
protected:
    void resizeEvent(QResizeEvent *event) override;
    
private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);
    
private:
    LineNumberArea *lineNumberArea;
    
    friend class LineNumberArea;
};

#endif // CODEEDITOR_H
