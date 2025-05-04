#include "gotolinedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QTextBlock>

GoToLineDialog::GoToLineDialog(QWidget *parent)
    : QDialog(parent), editor(nullptr)
{
    setWindowTitle("Go to Line");
    setMinimumWidth(300);

    // Create form layout for line number input
    QFormLayout *formLayout = new QFormLayout;

    // Line number input
    lineNumberSpinBox = new QSpinBox(this);
    lineNumberSpinBox->setMinimum(1);
    lineNumberSpinBox->setMaximum(1);
    formLayout->addRow("Line number:", lineNumberSpinBox);

    // Buttons layout
    QHBoxLayout *buttonsLayout = new QHBoxLayout;

    // Go button
    goButton = new QPushButton("Go", this);
    goButton->setDefault(true);
    buttonsLayout->addWidget(goButton);

    // Close button
    closeButton = new QPushButton("Close", this);
    buttonsLayout->addWidget(closeButton);

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonsLayout);

    // Connect signals and slots
    connect(goButton, &QPushButton::clicked, this, &GoToLineDialog::goToLine);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::close);
}

void GoToLineDialog::setEditor(QPlainTextEdit *editor)
{
    this->editor = editor;
    
    if (editor) {
        // Set max line to document's block count
        lineNumberSpinBox->setMaximum(editor->document()->blockCount());
        
        // Set current value to the current cursor line
        QTextCursor cursor = editor->textCursor();
        int currentLine = cursor.blockNumber() + 1; // +1 because block numbers are 0-based
        lineNumberSpinBox->setValue(currentLine);
    } else {
        lineNumberSpinBox->setMaximum(1);
        lineNumberSpinBox->setValue(1);
    }
    
    // Enable/disable the Go button based on whether we have an editor
    goButton->setEnabled(editor != nullptr);
}

void GoToLineDialog::goToLine()
{
    if (!editor)
        return;
    
    int lineNumber = lineNumberSpinBox->value() - 1; // -1 because block numbers are 0-based
    
    if (lineNumber >= 0 && lineNumber < editor->document()->blockCount()) {
        QTextCursor cursor(editor->document()->findBlockByNumber(lineNumber));
        editor->setTextCursor(cursor);
        editor->centerCursor(); // Make sure the line is visible
        editor->setFocus();
    }
}
