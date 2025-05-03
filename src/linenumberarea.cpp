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
    painter.fillRect(event->rect(), Qt::lightGray);
    
    QTextBlock block = codeEditor->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(codeEditor->blockBoundingGeometry(block).translated(codeEditor->contentOffset()).top());
    int bottom = top + qRound(codeEditor->blockBoundingRect(block).height());
    
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, width() - 2, codeEditor->fontMetrics().height(),
                             Qt::AlignRight, number);
        }
        
        block = block.next();
        top = bottom;
        bottom = top + qRound(codeEditor->blockBoundingRect(block).height());
        ++blockNumber;
    }
}
