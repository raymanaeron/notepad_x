#include "editorwidget.h"
#include <QVBoxLayout>
#include <QTextBlock>

EditorWidget::EditorWidget(QWidget *parent) : QWidget(parent)
{
    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Create text editor
    textEditor = new CodeEditor(this);
    setupEditor();
    
    // Add widgets to layout
    layout->addWidget(textEditor);
    
    setLayout(layout);
}

void EditorWidget::setupEditor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);
    textEditor->setFont(font);
    
    // Set tab width to 4 characters
    QFontMetrics metrics(font);
    textEditor->setTabStopDistance(4 * metrics.horizontalAdvance(' '));
    
    // Enable line wrapping
    textEditor->setLineWrapMode(QPlainTextEdit::NoWrap);
}
