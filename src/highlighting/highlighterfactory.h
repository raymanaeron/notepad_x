#ifndef HIGHLIGHTERFACTORY_H
#define HIGHLIGHTERFACTORY_H

#include "syntaxhighlighter.h"
#include "languages/cpphighlighter.h"
#include "languages/rusthighlighter.h"
#include "languages/gohighlighter.h"
#include "languages/pythonhighlighter.h"
#include "languages/csharphighlighter.h"
#include "languages/javahighlighter.h"
#include "languages/typescripthighlighter.h"
#include "languages/javascripthighlighter.h"
#include "languages/htmlhighlighter.h"
#include "languages/csshighlighter.h"
#include "languages/markuphighlighter.h"
#include <QMap>
#include <QFileInfo>

class HighlighterFactory
{
public:
    static HighlighterFactory& instance();
    
    // Create a highlighter based on file extension
    SyntaxHighlighter* createHighlighterForFile(const QString &filePath, QTextDocument *document);
    
    // Create a highlighter by language name
    SyntaxHighlighter* createHighlighter(const QString &language, QTextDocument *document);
    
    // Get supported languages
    QStringList supportedLanguages() const;
    
private:
    HighlighterFactory(); // Private constructor for singleton
    
    // Private helper to find language by extension
    QString languageForExtension(const QString &extension);
    
    // Store available languages
    QMap<QString, LanguageData*> m_languages;
    
    // Map of extensions to language names
    QMap<QString, QString> m_extensionMap;
};

#endif // HIGHLIGHTERFACTORY_H
