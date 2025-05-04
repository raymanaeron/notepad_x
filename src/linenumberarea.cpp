#include "linenumberarea.h"
#include "codeeditor.h"
#include <QPainter>
#include <QTextBlock>

LineNumberArea::LineNumberArea(CodeEditor *editor) 
    : QWidget(editor), codeEditor(editor)
{
}

QSize LineNumberArea::sizeHint() const
{
    return QSize(codeEditor->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    
    // Set VS Code-like background color based on theme
    if (codeEditor->isDarkTheme) {
        painter.fillRect(event->rect(), QColor(30, 30, 30)); // VS Code dark theme gutter
    } else {
        painter.fillRect(event->rect(), QColor(245, 245, 245)); // VS Code light theme gutter
    }
    
    // Use the same font as the editor for line numbers
    painter.setFont(codeEditor->font());
    
    QTextBlock block = codeEditor->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(codeEditor->blockBoundingGeometry(block).translated(codeEditor->contentOffset()).top());
    int bottom = top + qRound(codeEditor->blockBoundingRect(block).height());
    
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            
            // VS Code-like line number colors
            if (codeEditor->isDarkTheme) {
                painter.setPen(QColor(133, 133, 133)); // VS Code dark theme line numbers
            } else {
                painter.setPen(QColor(110, 110, 110)); // VS Code light theme line numbers
            }
            
            // Draw text with appropriate right margin (leave 30px gap)
            painter.drawText(0, top, width() - 30, codeEditor->fontMetrics().height(),
                            Qt::AlignRight, number);
        }
        
        block = block.next();
        top = bottom;
        bottom = top + qRound(codeEditor->blockBoundingRect(block).height());
        ++blockNumber;
    }
}
