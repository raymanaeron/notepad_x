#include "highlighterfactory.h"

HighlighterFactory& HighlighterFactory::instance()
{
    static HighlighterFactory factory;
    return factory;
}

HighlighterFactory::HighlighterFactory()
{
    // Register languages - "None" first to ensure it appears at the top
    m_languages["None"] = new LanguageData(); // Plain text, no highlighting
    m_languages["C++"] = new CppLanguage();
    m_languages["Rust"] = new RustLanguage();
    
    // Create extension mappings - populated from the language data
    for (auto it = m_languages.constBegin(); it != m_languages.constEnd(); ++it) {
        const QString& language = it.key();
        const LanguageData* langData = it.value();
        
        // Map each extension to this language
        for (const QString& ext : langData->fileExtensions()) {
            m_extensionMap[ext.toLower()] = language;
        }
    }
}

SyntaxHighlighter* HighlighterFactory::createHighlighterForFile(const QString &filePath, QTextDocument *document)
{
    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix().toLower();
    
    // Find language for this extension
    QString language = languageForExtension(extension);
    
    // Create highlighter for the language
    return createHighlighter(language, document);
}

SyntaxHighlighter* HighlighterFactory::createHighlighter(const QString &language, QTextDocument *document)
{
    // If language exists in our map
    if (m_languages.contains(language)) {
        return new SyntaxHighlighter(*m_languages[language], document);
    }
    
    // Otherwise return a plain text highlighter
    return new SyntaxHighlighter(document);
}

QString HighlighterFactory::languageForExtension(const QString &extension)
{
    if (m_extensionMap.contains(extension)) {
        return m_extensionMap[extension];
    }
    return "Plain Text";  // Default
}

QStringList HighlighterFactory::supportedLanguages() const
{
    // Ensure "None" appears first in the list
    QStringList languages = m_languages.keys();
    languages.removeAll("None");
    languages.sort();
    languages.prepend("None");
    return languages;
}
