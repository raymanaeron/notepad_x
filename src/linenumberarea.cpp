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
    
    // Set background color based on theme
    if (codeEditor->isDarkTheme) {
        painter.fillRect(event->rect(), QColor(45, 45, 45)); // Dark background
    } else {
        painter.fillRect(event->rect(), QColor(240, 240, 240)); // Light background
    }
    
    QTextBlock block = codeEditor->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(codeEditor->blockBoundingGeometry(block).translated(codeEditor->contentOffset()).top());
    int bottom = top + qRound(codeEditor->blockBoundingRect(block).height());
    
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            
            // Set text color based on theme
            if (codeEditor->isDarkTheme) {
                painter.setPen(QColor(180, 180, 180)); // Light gray text for dark theme
            } else {
                painter.setPen(Qt::black); // Black text for light theme
            }
            
            painter.drawText(0, top, width() - 5, codeEditor->fontMetrics().height(),
                            Qt::AlignRight, number);
        }
        
        block = block.next();
        top = bottom;
        bottom = top + qRound(codeEditor->blockBoundingRect(block).height());
        ++blockNumber;
    }
}
