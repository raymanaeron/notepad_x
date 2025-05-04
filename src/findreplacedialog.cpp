#include "findreplacedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QLabel>
#include <QMessageBox>

FindReplaceDialog::FindReplaceDialog(QWidget *parent)
    : QDialog(parent), editor(nullptr)
{
    setWindowTitle("Find and Replace");
    setMinimumWidth(400);

    // Create form layout for find/replace inputs
    QFormLayout *formLayout = new QFormLayout;

    // Find field
    findLineEdit = new QLineEdit(this);
    formLayout->addRow("Find:", findLineEdit);

    // Replace field
    replaceLineEdit = new QLineEdit(this);
    formLayout->addRow("Replace with:", replaceLineEdit);

    // Case sensitivity option
    caseSensitiveCheckBox = new QCheckBox("Case sensitive", this);
    formLayout->addRow("", caseSensitiveCheckBox);

    // Status label
    statusLabel = new QLabel(this);
    statusLabel->setStyleSheet("color: gray;");
    formLayout->addRow("", statusLabel);

    // Buttons layout
    QHBoxLayout *buttonsLayout = new QHBoxLayout;

    // Find next button
    findButton = new QPushButton("Find Next", this);
    findButton->setDefault(true);
    buttonsLayout->addWidget(findButton);

    // Replace button
    replaceButton = new QPushButton("Replace", this);
    buttonsLayout->addWidget(replaceButton);

    // Replace all button
    replaceAllButton = new QPushButton("Replace All", this);
    buttonsLayout->addWidget(replaceAllButton);

    // Close button
    closeButton = new QPushButton("Close", this);
    buttonsLayout->addWidget(closeButton);

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonsLayout);

    // Connect signals and slots
    connect(findButton, &QPushButton::clicked, this, &FindReplaceDialog::findNext);
    connect(replaceButton, &QPushButton::clicked, this, &FindReplaceDialog::replace);
    connect(replaceAllButton, &QPushButton::clicked, this, &FindReplaceDialog::replaceAll);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::close);
    connect(findLineEdit, &QLineEdit::textChanged, this, &FindReplaceDialog::updateUI);

    // Initial UI state
    updateUI();
}

void FindReplaceDialog::setEditor(QPlainTextEdit *editor)
{
    this->editor = editor;
    
    // Update find text with current selection
    if (editor) {
        QTextCursor cursor = editor->textCursor();
        if (cursor.hasSelection()) {
            findLineEdit->setText(cursor.selectedText());
        }
    }
    
    updateUI();
}

void FindReplaceDialog::findNext()
{
    find(true, true);
}

void FindReplaceDialog::replace()
{
    if (!editor)
        return;

    QTextCursor cursor = editor->textCursor();
    
    // If there's a selection and it matches find text, replace it
    if (cursor.hasSelection()) {
        QString selectedText = cursor.selectedText();
        QString findText = findLineEdit->text();
        
        // Check if case sensitive
        if ((!caseSensitiveCheckBox->isChecked() && 
             selectedText.compare(findText, Qt::CaseInsensitive) == 0) ||
            (caseSensitiveCheckBox->isChecked() && 
             selectedText == findText)) {
            
            cursor.beginEditBlock();
            cursor.removeSelectedText();
            cursor.insertText(replaceLineEdit->text());
            cursor.endEditBlock();
            editor->setTextCursor(cursor);
        }
    }
    
    // Find the next occurrence
    find();
}

void FindReplaceDialog::replaceAll()
{
    if (!editor || findLineEdit->text().isEmpty())
        return;
    
    QTextCursor originalCursor = editor->textCursor();
    editor->moveCursor(QTextCursor::Start);
    
    int replacements = 0;
    
    editor->document()->blockSignals(true);
    
    QTextCursor cursor = editor->textCursor();
    cursor.beginEditBlock();
    
    // Find and replace all occurrences
    while (find(true, false)) {
        QTextCursor currentCursor = editor->textCursor();
        currentCursor.removeSelectedText();
        currentCursor.insertText(replaceLineEdit->text());
        replacements++;
    }
    
    cursor.endEditBlock();
    
    editor->document()->blockSignals(false);
    
    // Restore cursor position
    editor->setTextCursor(originalCursor);
    
    // Show summary
    if (replacements > 0) {
        statusLabel->setText(QString("Replaced %1 occurrence(s)").arg(replacements));
    } else {
        statusLabel->setText("No matches found");
    }
}

bool FindReplaceDialog::find(bool forward, bool showError)
{
    if (!editor || findLineEdit->text().isEmpty())
        return false;
    
    QTextDocument::FindFlags flags;
    
    if (caseSensitiveCheckBox->isChecked())
        flags |= QTextDocument::FindCaseSensitively;
    
    if (!forward)
        flags |= QTextDocument::FindBackward;
    
    bool found = editor->find(findLineEdit->text(), flags);
    
    // If not found from current position and we're searching forward, try from beginning
    if (!found && forward) {
        QTextCursor cursor = editor->textCursor();
        cursor.movePosition(QTextCursor::Start);
        editor->setTextCursor(cursor);
        
        found = editor->find(findLineEdit->text(), flags);
    }
    
    if (found) {
        statusLabel->setText("");
    } else if (showError) {
        statusLabel->setText("No matches found");
    }
    
    return found;
}

void FindReplaceDialog::updateUI()
{
    bool hasText = !findLineEdit->text().isEmpty();
    bool hasEditor = editor != nullptr;
    
    findButton->setEnabled(hasText && hasEditor);
    replaceButton->setEnabled(hasText && hasEditor);
    replaceAllButton->setEnabled(hasText && hasEditor);
}
