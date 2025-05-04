#include "editorwidget.h"
#include "codeeditor.h"  // Add this include
#include "highlighting/highlighterfactory.h"
#include <QVBoxLayout>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QFontDatabase>

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
    // Use platform-specific fonts that match VS Code defaults
    QFont font;
    
    // Platform-specific font selection
    #if defined(Q_OS_WIN)
        // Windows - VS Code default is Consolas
        font.setFamily("Consolas");
    #elif defined(Q_OS_MACOS)
        // macOS - VS Code default is Menlo
        font.setFamily("Menlo");
    #elif defined(Q_OS_LINUX)
        // Linux - VS Code defaults to Ubuntu Mono or DejaVu Sans Mono
        QStringList linuxFonts = {"Ubuntu Mono", "DejaVu Sans Mono"};
        bool fontFound = false;
        
        // Try to find a known font that exists on the system
        QFontDatabase fontDatabase;
        for (const QString &fontFamily : linuxFonts) {
            if (fontDatabase.families().contains(fontFamily, Qt::CaseInsensitive)) {
                font.setFamily(fontFamily);
                fontFound = true;
                break;
            }
        }
        
        // If none found, fall back to any monospace
        if (!fontFound) {
            font.setFamily("monospace");
        }
    #else
        // Generic fallback for other platforms
        font.setFamily("monospace");
    #endif
    
    // Set fallback options for all platforms
    font.setStyleHint(QFont::Monospace); 
    font.setFixedPitch(true);
    font.setPointSize(11); // Slightly larger for better readability
    
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
    
    // Set VS Code-like dark theme colors for editor
    QPalette darkEditorPalette;
    darkEditorPalette.setColor(QPalette::Base, QColor(30, 30, 30));      // VS Code dark background
    darkEditorPalette.setColor(QPalette::Text, QColor(220, 220, 220));   // VS Code light text
    textEditor->setPalette(darkEditorPalette);
    
    // Custom VS Code-like dark background color for editor
    textEditor->setStyleSheet("QPlainTextEdit { background-color: #1E1E1E; color: #DCDCDC; }");
    
    // Update highlighter with dark theme colors
    if (highlighter) {
        highlighter->setDarkTheme(true);
    }
    
    // Update line number area highlighting
    textEditor->updateLineNumberAreaForTheme(true);
}

void EditorWidget::undo()
{
    if (textEditor) textEditor->undo();
}

void EditorWidget::redo()
{
    if (textEditor) textEditor->redo();
}

void EditorWidget::cut()
{
    if (textEditor) textEditor->cut();
}

void EditorWidget::copy()
{
    if (textEditor) textEditor->copy();
}

void EditorWidget::paste()
{
    if (textEditor) textEditor->paste();
}

void EditorWidget::selectAll()
{
    if (textEditor) textEditor->selectAll();
}

void EditorWidget::zoomIn(int range)
{
    if (textEditor) {
        textEditor->zoomIn(range);
        emit zoomLevelChanged(textEditor->getCurrentZoomLevel());
    }
}

void EditorWidget::zoomOut(int range)
{
    if (textEditor) {
        textEditor->zoomOut(range);
        emit zoomLevelChanged(textEditor->getCurrentZoomLevel());
    }
}

void EditorWidget::resetZoom()
{
    if (textEditor) {
        textEditor->resetZoom();
        emit zoomLevelChanged(textEditor->getCurrentZoomLevel());
    }
}

int EditorWidget::getCurrentZoomLevel() const
{
    return textEditor ? textEditor->getCurrentZoomLevel() : 0;
}

void EditorWidget::setZoomLevel(int level)
{
    if (textEditor) {
        textEditor->setZoomLevel(level);
    }
}
