#include "editorwidget.h"
#include "highlighting/highlighterfactory.h"
#include <QVBoxLayout>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

EditorWidget::EditorWidget(QWidget *parent) : QWidget(parent), curFile(""), isDarkTheme(false)
{
    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Create text editor
    textEditor = new CodeEditor(this);
    setupEditor();
    
    // Create syntax highlighter with appropriate language
    highlighter = nullptr;
    updateHighlighter();
    
    // Add widgets to layout
    layout->addWidget(textEditor);
    
    setLayout(layout);
    
    // Connect signals for document modification
    connect(textEditor->document(), &QTextDocument::contentsChanged,
            this, &EditorWidget::documentWasModified);
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

bool EditorWidget::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Notepad X",
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName))
                             .arg(file.errorString()));
        return false;
    }
    
    QTextStream in(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    in.setEncoding(QStringConverter::Utf8);
#else
    in.setCodec("UTF-8");
#endif
    
    textEditor->setPlainText(in.readAll());
    
    setCurrentFile(fileName);
    updateHighlighter();  // Update highlighter based on file extension
    return true;
}

bool EditorWidget::save()
{
    if (isUntitled()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool EditorWidget::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save As", curFile.isEmpty() ? 
                                                  QDir::homePath() : curFile);
    if (fileName.isEmpty())
        return false;
        
    return saveFile(fileName);
}

bool EditorWidget::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Notepad X",
                           tr("Cannot write file %1:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName))
                           .arg(file.errorString()));
        return false;
    }
    
    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif
    
    out << textEditor->toPlainText();
    
    setCurrentFile(fileName);
    return true;
}

void EditorWidget::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    textEditor->document()->setModified(false);
    emit fileNameChanged(curFile);
    emit modificationChanged(false);
}

void EditorWidget::updateHighlighter()
{
    // Delete old highlighter if it exists
    if (highlighter) {
        delete highlighter;
        highlighter = nullptr;
    }
    
    // Create new highlighter based on file extension
    if (!curFile.isEmpty()) {
        highlighter = HighlighterFactory::instance().createHighlighterForFile(curFile, textEditor->document());
    } else {
        // Default highlighter for new files - None (plain text)
        highlighter = HighlighterFactory::instance().createHighlighter("None", textEditor->document());
    }
    
    emit languageChanged(highlighter->languageName());
}

void EditorWidget::setLanguage(const QString &language)
{
    if (highlighter) {
        delete highlighter;
    }
    
    highlighter = HighlighterFactory::instance().createHighlighter(language, textEditor->document());
    
    // Apply theme-specific colors if we're in dark mode
    if (isDarkTheme && highlighter) {
        highlighter->setDarkTheme(true);
    }
    
    emit languageChanged(highlighter ? highlighter->languageName() : "Plain Text");
}

QString EditorWidget::currentLanguage() const
{
    return highlighter ? highlighter->languageName() : "Plain Text";
}

void EditorWidget::documentWasModified()
{
    emit modificationChanged(textEditor->document()->isModified());
}

bool EditorWidget::isModified() const
{
    return textEditor->document()->isModified();
}

void EditorWidget::setModified(bool modified)
{
    textEditor->document()->setModified(modified);
}

bool EditorWidget::maybeSave()
{
    if (!isModified())
        return true;
        
    const QMessageBox::StandardButton ret =
        QMessageBox::warning(this, "Notepad X",
                           "The document has been modified.\n"
                           "Do you want to save your changes?",
                           QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
                           
    if (ret == QMessageBox::Save)
        return save();
    else if (ret == QMessageBox::Cancel)
        return false;
        
    return true;  // Discard was clicked
}

void EditorWidget::setLightTheme()
{
    isDarkTheme = false;
    
    // Set light theme colors for editor
    QPalette editorPalette;
    textEditor->setPalette(editorPalette);
    
    // Default light background color for editor
    textEditor->setStyleSheet("QPlainTextEdit { background-color: #ffffff; color: #000000; }");
    
    // Update highlighter with light theme colors
    if (highlighter) {
        highlighter->setDarkTheme(false);
    }
    
    // Update line number area highlighting
    textEditor->updateLineNumberAreaForTheme(false);
}

void EditorWidget::setDarkTheme()
{
    isDarkTheme = true;
    
    // Set dark theme colors for editor
    QPalette darkEditorPalette;
    darkEditorPalette.setColor(QPalette::Base, QColor(35, 35, 35));
    darkEditorPalette.setColor(QPalette::Text, Qt::white);
    textEditor->setPalette(darkEditorPalette);
    
    // Custom dark background color for editor
    textEditor->setStyleSheet("QPlainTextEdit { background-color: #232323; color: #ffffff; }");
    
    // Update highlighter with dark theme colors
    if (highlighter) {
        highlighter->setDarkTheme(true);
    }
    
    // Update line number area highlighting
    textEditor->updateLineNumberAreaForTheme(true);
}
