#include "codeeditor.h"
#include "linenumberarea.h"
#include <QPainter>
#include <QTextBlock>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent), isDarkTheme(false), zoomLevel(0)
{
    lineNumberArea = new LineNumberArea(this);
    
    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);
    
    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

void CodeEditor::setEditorMargins(int left, int top, int right, int bottom)
{
    setViewportMargins(left, top, right, bottom);
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    
    // Add extra padding (30px) to match VS Code's appearance
    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits + 30;
    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setEditorMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    
    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), 
                                      lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        
        // VS Code-like current line highlighting
        QColor lineColor = isDarkTheme ? 
                         QColor(40, 40, 40) :      // VS Code dark theme current line
                         QColor(240, 240, 240);    // VS Code light theme current line
        
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }
    
    setExtraSelections(extraSelections);
}

void CodeEditor::updateLineNumberAreaForTheme(bool isDark)
{
    isDarkTheme = isDark;
    
    // Update current line highlight with new theme colors
    highlightCurrentLine();
    
    // Force repaint of line number area
    lineNumberArea->update();
}

// Add the zoom methods
void CodeEditor::zoomIn(int range)
{
    zoomLevel += range;
    QFont font = this->font();
    font.setPointSize(DEFAULT_FONT_SIZE + zoomLevel);
    this->setFont(font);
    
    // Update line number area width after font size change
    updateLineNumberAreaWidth(0);
    lineNumberArea->setFont(font);
}

void CodeEditor::zoomOut(int range)
{
    zoomLevel -= range;
    // Don't allow extremely small fonts
    if (DEFAULT_FONT_SIZE + zoomLevel < 6) {
        zoomLevel = 6 - DEFAULT_FONT_SIZE;
    }
    
    QFont font = this->font();
    font.setPointSize(DEFAULT_FONT_SIZE + zoomLevel);
    this->setFont(font);
    
    // Update line number area width after font size change
    updateLineNumberAreaWidth(0);
    lineNumberArea->setFont(font);
}

void CodeEditor::resetZoom()
{
    zoomLevel = 0;
    QFont font = this->font();
    font.setPointSize(DEFAULT_FONT_SIZE);
    this->setFont(font);
    
    // Update line number area width after font size change
    updateLineNumberAreaWidth(0);
    lineNumberArea->setFont(font);
}

void CodeEditor::setZoomLevel(int level)
{
    if (level != zoomLevel) {
        zoomLevel = level;
        // Don't allow extremely small fonts
        if (DEFAULT_FONT_SIZE + zoomLevel < 6) {
            zoomLevel = 6 - DEFAULT_FONT_SIZE;
        }
        
        QFont font = this->font();
        font.setPointSize(DEFAULT_FONT_SIZE + zoomLevel);
        this->setFont(font);
        
        // Update line number area width after font size change
        updateLineNumberAreaWidth(0);
        lineNumberArea->setFont(font);
    }
}

// Override wheel event to handle Ctrl+mouse wheel zoom
void CodeEditor::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        if (e->angleDelta().y() > 0) {
            zoomIn();
        } else {
            zoomOut();
        }
        e->accept();
    } else {
        QPlainTextEdit::wheelEvent(e);
    }
}
