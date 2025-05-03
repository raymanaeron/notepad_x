#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QWidget>

// Forward declaration to avoid circular includes
class CodeEditor;

class LineNumberArea : public QWidget
{
    Q_OBJECT
    
public:
    // Constructor declaration only (implementation in cpp file)
    explicit LineNumberArea(CodeEditor *editor);
    
    // Size hint declaration (implementation in cpp file)
    QSize sizeHint() const override;
    
protected:
    void paintEvent(QPaintEvent *event) override;
    
private:
    CodeEditor *codeEditor;
};

#endif // LINENUMBERAREA_H
